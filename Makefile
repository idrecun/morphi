CXX=g++

SOURCES=morphi.cpp automorphism_set.cpp colouring.cpp graph.cpp permutation.cpp utility.cpp search.cpp
OBJECTS=$(subst .cpp,.o,$(SOURCES))

all: morphi

morphi: $(OBJECTS)
	$(CXX) -O3 $(OBJECTS) -o morphi -pg

%.o: %.cpp %.hpp
	$(CXX) -O3 -c $<

clean:
	$(RM) $(OBJECTS)
	$(RM) morphi
