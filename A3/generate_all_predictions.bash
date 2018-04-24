#!/bin/bash

cd data

for f in * 
do
    #echo $f
    mv $f ../query
    ../bin/movie_recommender ../query/$f *
    mv ../query/$f .
done

cd -
