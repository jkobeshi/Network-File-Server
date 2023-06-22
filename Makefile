CC=g++ -g -Wall -std=c++17

# List of source files for your file server
FS_SOURCES=fs_server.cpp

# Generate the names of the file server's object files
FS_OBJS=${FS_SOURCES:.cpp=.o}

all: fs appspec app1 app2 app3 app4 app5 app6 app7 app8 app9 app10 app11 app12 app13 app14 app15 app16 app17 app18 app19

# Compile the file server and tag this compilation
fs: ${FS_OBJS} libfs_server.o
	./autotag.sh
	${CC} -o $@ $^ -pthread -ldl

# Compile a client program
app1: test1.cpp libfs_client.o
	${CC} -o $@ $^

app2: test2.cpp libfs_client.o
	${CC} -o $@ $^

app3: test3.cpp libfs_client.o
	${CC} -o $@ $^

app4: test4.cpp libfs_client.o
	${CC} -o $@ $^
	
app5: test5.cpp libfs_client.o
	${CC} -o $@ $^

app6: test6.cpp libfs_client.o
	${CC} -o $@ $^

app7: test7.cpp libfs_client.o
	${CC} -o $@ $^

app8: test8.cpp libfs_client.o
	${CC} -o $@ $^
	
app9: test9.cpp libfs_client.o
	${CC} -o $@ $^
		
app10: test10.cpp libfs_client.o
	${CC} -o $@ $^

app11: test11.cpp libfs_client.o
	${CC} -o $@ $^

app12: test12.cpp libfs_client.o
	${CC} -o $@ $^

app13: test13.cpp libfs_client.o
	${CC} -o $@ $^

app14: test14.cpp libfs_client.o
	${CC} -o $@ $^

app15: test15.cpp libfs_client.o
	${CC} -o $@ $^

app16: test16.cpp libfs_client.o
	${CC} -o $@ $^

app17: test17.cpp libfs_client.o
	${CC} -o $@ $^

app18: test18.cpp libfs_client.o
	${CC} -o $@ $^

app19: test19.cpp libfs_client.o
	${CC} -o $@ $^

appspec: testspec.cpp libfs_client.o
	${CC} -o $@ $^
# Generic rules for compiling a source file to an object file
%.o: %.cpp
	${CC} -c $<
%.o: %.cc
	${CC} -c $<

clean:
	rm -f ${FS_OBJS} fs appspec app1 app2 app3 app4 app5 app6 app7 app8 app9 app10 app11 app12 app13 app14 app15 app16 app17 app18 app19
