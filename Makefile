CXX=g++

SOURCES=morphi.cpp automorphism_set.cpp colouring.cpp graph.cpp permutation.cpp utility.cpp
OBJECTS=$(subst .cpp,.o,$(SOURCES))

all: morphi

morphi: $(OBJECTS)
	$(CXX) $(OBJECTS) -o morphi -pg

%.o: %.cpp %.hpp
	$(CXX) -c $<

clean:
	$(RM) $(OBJECTS)
	$(RM) morphi
