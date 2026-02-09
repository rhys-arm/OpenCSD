This is a small image (25 instructions) for running on an A-class core
on a Juno target. Its load address is set as 0x80000000 (appropriate
for the Juno's memory map). This can be used for generating trace from
the ETMv4 and comparing against the golden trace report.

golden_trace_report.txt was generated with:
trace report COLUMNS=RECORD_TYPE,ADDRESS,OPCODE FORMAT=CSV HEADERS=true FILE=golden_trace_report.txt

Note that the DETAIL column is not included because this is trace independent,
and can vary depending on the disassembler.


__Building__

The .axf file can be rebuilt using the compiler utilities shipped with
Arm DS.

On Windows, open an Arm DS Command Prompt from the Start menu, run the
select_toolchain utility, and select Arm Compiler for Embedded 6 from the list.

On Linux, run the suite_exec utility with the --toolchain option to
select the compiler and start a shell configured for the Development
Studio environment, for example:
~/developmentstudio/bin/suite_exec --toolchain "Arm Compiler for Embedded 6" bash

Then in this directory type 'make' or 'make clean'.
