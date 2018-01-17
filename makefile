CC=g++
CFLAGS=-pthread
DBFLAG=-g
FASTFLAGS=-Ofast

app:
	$(CC) -std=c++0x -o App main.cc /usr/include/halfedge.cc $(FASTFLAGS) -fomit-frame-pointer $(ENGINE) $(CFLAGS)

app-db:
	$(CC) -std=c++0x -o App main.cc /usr/include/halfedge.cc $(ENGINE) $(CFLAGS) $(DBFLAG)

app-op:
	$(CC) -std=c++0x -o App main.cc /usr/include/halfedge.cc $(ENGINE) $(CFLAGS) $(DBFLAG)

app-prof:
	$(CC) -std=c++0x -o App main.cc /usr/include/halfedge.cc $(FASTFLAGS) $(ENGINE) $(CFLAGS) $(DBFLAG)
	
