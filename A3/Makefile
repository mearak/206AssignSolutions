SRC=src/movie_recommender.c src/preferences.c src/distances.c 
HEADERS=include/preferences.h include/distances.h

all: bin/movie_recommender

bin/movie_recommender: ${SRC} ${HEADERS} 
	gcc -Iinclude ${SRC} -o bin/movie_recommender

clean: 
	rm bin/movie_recommender
