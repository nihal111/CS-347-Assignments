import random

for file_no in range(1,11):
	string = ""
	for district in range(20*(file_no-1) + 1, 20*(file_no) + 1):
		string += str(district) + "\n"
		parties = random.sample(range(1, 21), 5)
		if not 1 in parties:
			parties[0] = 1
		seats = random.sample(range(1, 10), 5)

		for i in range(len(parties)):
			string += str(parties[i]) + " " + str(seats[i]) + "\n"

		string += "\n"

	text_file = open("file"+str(file_no)+".txt", "w")
	text_file.write(string)
	text_file.close()