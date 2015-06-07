drop procedure str_to_table(char, varchar(any));
drop table str_to_table_reftable;

create table str_to_table_reftable(num int, value varchar(4000));

-- Converts string to table representation.
-- Usage str_to_table(<delimeter>, <string>).
create or replace procedure str_to_table(char, varchar(any)) 
	returns reftable(str_to_table_reftable)
	language nzplsql as
begin_proc
declare
	delim alias for $1;
	str alias for $2;
	current_str varchar(4101);
	str_length int;
	str_s int;
	str_e int;
	str_sep int;
	str_num int;
begin
	str_length := length(str);
	str_s := 1;
	str_num := 1;

	while str_s <= str_length loop
		str_e := position(delim in substr(str, str_s));

		if str_e = 0 then
			str_e := str_length + 1;
		else
			str_e := str_e + str_s - 1;
		end if;

		current_str := substr(str, str_s, str_e - str_s);

		execute immediate 'insert into ' || reftablename || ' values (' || str_num || ', ''' || current_str || ''')';

		str_s := str_e + 1;
		str_num := str_num + 1;
	end loop;

	return reftable;
end;
end_proc;
