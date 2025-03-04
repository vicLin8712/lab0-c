# Demonstration of queue testing framework
# Use help command to see list of commands and options
# Initial queue is NULL.
show
# Create empty queue
new
# See how long it is
size
# Fill it with some values.  First at the head
ih dolphin
ih bear
ih gerbil
# Now at the tail
it meerkat
it bear
# Reverse it
reverse
# See how long it is
size
# Delete queue.  Goes back to a NULL queue.
free
# test remaining function
new
ih 1
ih 7
ih 40
ih 4
ih 5
ih 63
ih 7
ih 82
ih 32
ascend
desecnd



# Exit program
quit
