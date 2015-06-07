drop procedure args_to_table(varchar(any));
drop table args_to_table_reftable;

create table args_to_table_reftable(name varchar(100), value varchar(4000));

-- Converts arguments string to table representation.
-- Arguments string must be formatted as 'name1=value1, name2=value2, ...'.
create or replace procedure args_to_table(varchar(any)) 
	returns reftable(args_to_table_reftable)
	language nzplsql as
begin_proc
declare
	args alias for $1;
	arg_name varchar(100);
	arg_value varchar(4000);
	arg_sep int;
	arg_rec record;
begin
	for arg_rec in select str_to_table(',', args) loop
		if arg_rec.value is null or arg_rec.value = '' then
			raise exception 'Argument #% is empty.', arg_rec.num;
		end if;

		arg_sep := position('=' in arg_rec.value);
		if arg_sep = 0 then
			raise exception 'Argument #% (''%'') expected to have name and value separated with ''=''.', arg_rec.num, arg_rec.value;
		end if;

		arg_name := trim(substr(arg_rec.value, 1, arg_sep - 1));
		arg_value := trim(substr(arg_rec.value, arg_sep + 1));

		execute immediate 'insert into ' || reftablename || ' values (''' || arg_name || ''', ''' || arg_value || ''')';
	end loop;

	return reftable;
end;
end_proc;
