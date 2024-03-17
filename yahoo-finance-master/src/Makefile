main: example.o quote.o time_utils.o curl_utils.o spot.o
	g++ quote.o time_utils.o curl_utils.o example.o spot.o -o main -lcurl
quote.o: quote.cpp
	g++ -c quote.cpp
time_utils.o: time_utils.cpp
	g++ -c time_utils.cpp
curl_utils.o: curl_utils.cpp
	g++ -c curl_utils.cpp
example.o: example.cpp
	g++ -c example.cpp
spot.o: spot.cpp
	g++ -c spot.cpp
clean:
	rm *.o main
