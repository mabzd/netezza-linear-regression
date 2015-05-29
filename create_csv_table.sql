-- Creates table imported from CSV file
-- Usage create_csv_table(<table_name>, <cols>, <file_path>, <delim>)
create or replace procedure create_csv_table(varchar(any), int, varchar(any), char) 
	returns reftable(str_to_table_reftable)
	language nzplsql as
begin_proc
declare
	table_name alias for $1;
	cols alias for $2;
	file_path alias for $3;
	delim alias for $4;
	create_table_sql varchar(4000);
begin
	create_table_sql := 'create table ' || table_name || '(';
	for i in 1 .. (cols-1) loop
		create_table_sql := create_table_sql || 'x' || i || ' double, ';
	end loop;
	create_table_sql := create_table_sql || 'x' || cols || ' double);';

	execute immediate create_table_sql;

	execute immediate 'insert into ' || table_name || ' select * from external ''' || file_path || ''' using(delimiter ''' || delim || ''');';
end;
end_proc;