##
# Warship
#
# @file
# @version 0.1

main:
	mkdir target
	g++ -Wall -g main.cpp player.cpp -o target/warship -lncurses

clean:
	rm -r target

# end
