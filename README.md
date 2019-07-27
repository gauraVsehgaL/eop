# eop
Escalation of Privelages using named pipe
Run program as admin.

write to named pipe from a process running in service context(psexec -s -i cmd and echo eop>><pipe_name>).
