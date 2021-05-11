all: filter
filter:
	g++ -lsystemc -o filter main.cpp generator.cpp IIR_ca.cpp IIR_ca_filter_response.cpp IIR_tlm.cpp tb.cpp

clean:
	rm filter 
