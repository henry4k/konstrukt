- Zugriff auf lokale Variablen ist schneller als auf Globale.
  Das liegt daran, dass globale Variablen immer in einer Hashmap nachgeschaut werden müssen.
  Lokale Variablen sind dagegen in VM Registern, welche ledeglich über einen Index angesprochen werden.

- Speicherallokalisierung ist langsam.
  Das ist aber überall so.
  Also nach Möglichkeit vorher allokalisieren.

- In C lua_raw* in kritischen Bereichen nutzen.
  Normalerweise wird bei Lookups der Metatable beachtet. lua_raw* ignoriert diesen.

- String concatenation (a..b oder strsub() usw) ist langsam.
  Strings sind in Lua immer einzigartig. D.h. immer wenn ein neuer String kreiert wird,
  muss dieser in einem internen Table abgeglichen werden.

- Sprachkonstrukte sind schneller als Funktionen.
  Z.B. statt ipairs 'for k=1, #tbl do local v = tbl[k];'

- NUR DORT OPTIMIEREN WO ES AUCH NOTWENDIG IST!

- Strings als Enums nutzen.
