.Dd May 30, 2023
.Dt GPASS 1
.Os
.Sh NAME
.Nm gpass
.Nd generate passphrases
.Sh SYNOPSIS
.Nm
.Op Fl d Ar dict
.Op Fl e Ar bits
.Op Fl n Ar num
.Sh DESCRIPTION
The utility
.Nm
outputs a secure passphrase by randomly choosing an appropriate number of lines
from a dictionary file, containing whitespace-separated words.
The options are as follows:
.Bl -tag -width Ds
.It Fl d Ar dict
Use the dictionary
.Ar dict .
.It Fl e Ar bits
Generate a passphrase with at least the specified number of
.Ar bits
of entropy.
Default is 70.
.It Fl n Ar num
Generate
.Ar num
passphrases instead of just one.
.El
.Sh ENVIRONMENT
.Bl -tag -width PM_PRIVKEY
.It Ev GPASS_DIC
Path to the dictionary file to use unless the option
.Fl d
is specified.
.El
.Sh FILES
.Bl -tag -width Ds
.It Pa PREFIX/share/gpass/eff.long
The default dictionary file.
.El
.Sh AUTHORS
.An Alexander Arkhipov Aq Mt aa@manpager.net .
.Sh EXIT STATUS
.Ex -std