drop procedure linear_regression(varchar(any));
drop table linear_regression_reftable;

create table linear_regression_reftable(b int, value double);

-- Calculates multivariate linear regression model.
-- Arguments must be formatted as: 'model=linear_model, input_table=<table_name>,
-- input_col=<col1_name>; <col2_name>; ..., target=<col_name>'.
-- Returns B-coefficients vector as set of rows: <coefficient number>, <coefficient value>.
create or replace procedure linear_regression(varchar(any)) 
	returns reftable(linear_regression_reftable)
	language nzplsql as
begin_proc
declare
	args alias for $1;
	workspace varchar(4000);
	input_table varchar(64);
	input_cols varchar(4000);
	target_col varchar(4000);
	input_cols_str varchar(4000) := '';
	cols_count int := 0;
	rec record;
begin
	create local temp table args_table as
	select args_to_table(args);

	if (select count(*) from args_table) != 4 then
		raise exception 'Expected 4 arguments: model=linear_model, input_table=<table_name>, input_col=<col1_name>; <col2_name>; ..., target=<col_name>.';
	end if;

	select value into workspace
	from args_table 
	where lower(name) = 'model'
	limit 1;

	if not found or workspace != 'linear_model' then
		raise exception 'Expected ''linear_model'' as model.';
	end if;

	select value into input_table
	from args_table
	where lower(name) = 'input_table'
	limit 1;

	if not found or input_table = '' then
		raise exception 'Argument ''input_table'' not found.';
	end if;

	select value into input_cols
	from args_table
	where lower(name) = 'input_col'
	limit 1;

	if not found or input_cols = '' then
		raise exception 'Argument ''input_col'' not found.';
	end if;

	select value into target_col
	from args_table
	where lower(name) = 'target'
	limit 1;

	if not found or target_col = '' then
		raise exception 'Argument ''target'' not found.';
	end if;

	-- 1) calculate XTX matrix and send it to all SPUs
	for rec in select str_to_table(';', input_cols) order by num loop
		if rec.num != 1 then
			input_cols_str := input_cols_str || ',';
		end if;
		input_cols_str := input_cols_str || 't.' || trim(both ' ' from rec.value);
		cols_count := cols_count + 1;
	end loop;

	workspace := 'select xtx.x as x, xtx.y as y, sum(xtx.value) as value ' 
		|| 'from ' || input_table || ' t, table with final(xtxmatrix(' || input_cols_str || ', cast(1.0 as double))) xtx '
		|| 'group by xtx.x, xtx.y';

	execute immediate 'create temp table xtx_table as ' 
		|| 'select v.dsid as dsid, w.x as x, w.y as y, w.value as value ' 
		|| 'from (' || workspace || ') w, _v_dual_dslice v ' 
		|| 'distribute on (dsid)';

	-- 2) load XTX matrix to SPU memory
	execute immediate 'select sqmatrixpad(' || cols_count + 1 || ', x, y, value) '
		|| 'from xtx_table';

	-- 3) calculate XTX inverse on all SPUs
	execute immediate 'select invsqmatrixpad() from _v_dual_dslice';

	-- 4) calculate final B-coefficients vector
	execute immediate  'insert into ' || reftablename || ' '
		|| 'select f.b as b, sum(f.value) as value '
		|| 'from ' || input_table || ' t, table with final(calclrbvec(' || input_cols_str || ', cast(1.0 as double), t.' || target_col || ')) f '
		|| 'group by f.b';

	drop table xtx_table;
	drop table args_table;
	return reftable;
end;
end_proc;
