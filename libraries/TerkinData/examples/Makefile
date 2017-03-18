all: csv urlencoded json

run:
	@./csv_basic
	@./json_basic
	@./urlencoded_basic

csv:
	@./build csv_basic.cpp
	@./csv_basic

json:
	@./build json_basic.cpp
	@./json_basic

urlencoded:
	@./build urlencoded_basic.cpp
	@./urlencoded_basic

clean:
	rm csv_basic
	rm json_basic
	rm urlencoded_basic

