# README for dbtool 1.9.1 (21/11/2025)

dbtool can be used to store and retrieve data in a key/value
format in a hash database. Perl compatible regular expressions
are supported both for storing and retrieving of data. It's
main advantages are the ability to maintain huge amounts of
data and speed.

 


# Requirements

You need either the GNU gdbm library or the Berkeley database
system. You can find gdbm at:
  http://www.gnu.org/software/gdbm/gdbm.html
And the berkeley library at: http://www.sleepycat.com.

You will also need `meson` and `ninja`: https://mesonbuild.com/.

If you want to build the manual page, you'll need perl as well.



# Installation from source

Check out the source and execute:

```default
meson setup build
ninja -C build
```

You can supply some additional parameters to meson,
type `meson configuration` for details.

To install, execute:

```default
sudo ninja -C install
```


# Documentation

After successfully installation read the installed man-
page:

```default
man dbtool
```

You can also read the manual page [online](dbtool.pod).

# Report bugs

[Please open an issue](https://codeberg.org/scip/dbtool/issues). Thanks!


# License and Disclaimer

dbtool is Copyright (c) 2001-2025 by T.v. Dein. dbtool may be
used and distributed under the terms of the GNU General Public
License. All other brand and product names are trademarks,
registered trademarks or service marks of their respective
holders.

These programs are distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this distribution; if not, write to the


    Free Software Foundation, Inc.
    59 Temple Place
    Suite 330
    Boston, MA 02111
    USA





