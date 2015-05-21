create table args_to_table_reftable(name varchar(100), value varchar(4000));

-- Converts arguments string to table representation.
-- Arguments string must be formatted as 'name1=value1, name2=value2, ...'.
create or replace procedure args_to_table(varchar(any)) 
	returns reftable(args_to_table_reftable)
	language nzplsql as
begin_proc
declare
	args alias for $1;
	current_arg varchar(4101);
	current_arg_name varchar(100);
	current_arg_value varchar(4000);
	args_length int;
	arg_s int;
	arg_e int;
	arg_sep int;
	arg_num int;
begin
	args_length := length(args);
	arg_s := 1;
	arg_num := 1;

	while arg_s <= args_length loop
		arg_e := position(',' in substr(args, arg_s));

		if arg_e = 0 then
			arg_e := args_length + 1;
		else
			arg_e := arg_e + arg_s - 1;
		end if;

		current_arg := trim(substr(args, arg_s, arg_e - arg_s));

		if current_arg = '' then
			raise exception 'Argument #% is empty.', arg_num;
		end if;

		arg_sep := position('=' in current_arg);
		if arg_sep = 0 then
			raise exception 'Argument #% (''%'') expected to have name and value part separated with ''=''.', arg_num, current_arg;
		end if;

		current_arg_name := trim(substr(current_arg, 1, arg_sep - 1));
		current_arg_value := trim(substr(current_arg, arg_sep + 1));

		execute immediate 'insert into ' || reftablename || ' values (''' || current_arg_name || ''', ''' || current_arg_value || ''')';

		arg_s := arg_e + 1;
		arg_num := arg_num + 1;
	end loop;

	return reftable;
end;
end_proc;
