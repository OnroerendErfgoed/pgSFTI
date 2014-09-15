PostgreSQL simpleFuzzyTimeInterval
==================================

This PostgreSQL extension adds a simple FuzzyTimeInterval to PostgreSQL.

Installation
------------

Clone this repository or download it. Build the C extension:

```bash
$ sudo make install
/bin/mkdir -p '/usr/share/postgresql/9.1/extension'
/bin/mkdir -p '/usr/share/postgresql/9.1/extension'
/bin/mkdir -p '/usr/lib/postgresql/9.1/lib'
/bin/mkdir -p '/usr/share/doc/postgresql-doc-9.1/extension'
/bin/sh /usr/lib/postgresql/9.1/lib/pgxs/src/makefiles/../../config/install-sh -c -m 644 ./pgsfti.control '/usr/share/postgresql/9.1/extension/'
/bin/sh /usr/lib/postgresql/9.1/lib/pgxs/src/makefiles/../../config/install-sh -c -m 644 ././data/pgsfti--1.0.sql  '/usr/share/postgresql/9.1/extension/'
/bin/sh /usr/lib/postgresql/9.1/lib/pgxs/src/makefiles/../../config/install-sh -c -m 755  ./module/pgsfti.so '/usr/lib/postgresql/9.1/lib/'
/bin/sh /usr/lib/postgresql/9.1/lib/pgxs/src/makefiles/../../config/install-sh -c -m 644 ./README.md '/usr/share/doc/postgresql-doc-9.1/extension/'
```

This extension can now be added wihtin PostgreSQL to a database:

```sql
CREATE EXTENSION pgsfti;
```

If you want a nice set of testdata to go along with the installation, run the
test/test.sql script:

```bash
$ psql -U postgres testdatabase < test/test.sql
```
