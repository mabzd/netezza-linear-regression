create table linear_regression_reftable(b double);

-- Calculates multivariate linear regression model.
-- Arguments must be formatted as: 'model=linear_model, input_table=<table_name>,
-- input_col=<col1_name>; <col2_name>; ..., target=<col_name>'.
create or replace procedure linear_regression(varchar(any)) 
	returns reftable(linear_regression_reftable)
	language nzplsql as
begin_proc
declare
	args alias for $1;
	model varchar(4000);
	input_table varchar(64);
	input_cols varchar(4000);
	target_col varchar(4000);
begin
	create local temp table args_table as
	select * from args_to_table_reftable
	limit 0;

	insert into args_table
	select args_to_table(args);

	if (select count(*) from args_table) != 4 then
		raise exception 'Expected 4 arguments: model=linear_model, input_table=<table_name>, input_col=<col1_name>; <col2_name>; ..., target=<col_name>.';
	end if;

	select value into model
	from args_table 
	where lower(name) = 'model'
	limit 1;

	if not found or model != 'linear_model' then
		raise exception 'Expected ''linear_model'' as model.';
	end if;

	select value into input_table
	from args_table
	where lower(name) = 'input_table'
	limit 1;

	if not found then
		raise exception 'Argument ''input_table'' not found.';
	end if;

	select value into input_cols
	from args_table
	where lower(name) = 'input_col'
	limit 1;

	if not found then
		raise exception 'Argument ''input_col'' not found.';
	end if;

	select value into target_col
	from args_table
	where lower(name) = 'target'
	limit 1;

	if not found then
		raise exception 'Argument ''target'' not found.';
	end if;



	drop table args_table;
	return reftable;
end;
end_proc;
