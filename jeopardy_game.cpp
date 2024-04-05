
//CIS 439 Final Project Jeoparday Search Engine Game

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>//for sorting
#include <regex>
#include <list>
#include <cmath> //for log

/*

*/

//contains question data and the IDF score if the search option is uesd
struct Questions{
public:
	std::string question, answer, category, value, round, show_num, air_date;

	float score = 0;
	int question_num = 0;

	void init() {
		question = "";
		answer = "";
		category = "";
		value = "";
		round = ""; //one of "Jeopardy!","Double Jeopardy!","Final Jeopardy!" or "Tiebreaker"
		show_num = "";
		air_date = ""; //year-month-day
	}
};

//used for searching for IDF score
struct Words {
public:
	std::string word = "";
	int f = 0;//frewuency of word in question
	int k = 0;//question(document) frequency
	int question_num;
};

//used to sort by IDF score highest to lowest
bool compareScore(const Questions& a, const Questions& b)
{
	return a.score > b.score;
}

//sorts by question number
bool compareQuestionNum(const Questions& a, const Questions& b)
{
	return a.question_num > b.question_num;
}

//regeular expressions for detecting which type of quesion data is being read
std::regex category("(.*\"category\": \")");
std::regex question("(.*\"question\": \")");
std::regex show_number("(.*\"show_number\": \")");
std::regex value("(.*\"value\": \")");
std::regex air_date("(.*\"air_date\": \")");
std::regex answer("(.*\"answer\": \")");
std::regex round_("(.*\"round\": \")");

//reads the data set and inserts strings of data into the list
void getList(std::list<Questions>& list, std::string line, int amount) {

	Questions temp;
	temp.init();

	int total = 0;
	std::string type = "";

	for (int i = 0; i < line.length(); i++) {

			type += line[i];

			//check which type of data is about to be read, then read it until the a ",
			if (line[i] == '"') {

				if (regex_match(type, category)) {
					i++;
					while (1){
						if (line[i] == '"' && line[i + 1] == ',')//makes sure the comma and quote isnt included in the string
							break;
						else{
							temp.category += line[i];
							i++;
						}
					}
					type = "";
				}

				//questions can have all sorts of symbols and quotes so it need to specially verify that its the end of the question
				else if (regex_match(type, question)) {
					i++;
					while (1) {
						if (line[i] == '"' && line[i + 1] == ',' && line[i+4] == 'v') // also checks that v (start of "value") is 4 characters away because this means its most likely the end of the question
							break;
						else {
							temp.question += line[i];
							i++;
						}
					}

					//get rid of the ' character on each end of the question
					temp.question.erase(0,1);
					temp.question.erase(temp.question.length() - 1, 1);
					type = "";
				}

				else if (regex_match(type, answer)) {
					i++;
					while (1) {
						if (line[i] == '"' && line[i + 1] == ',')
							break;
						else {
							temp.answer += line[i];
							i++;
						}
					}
					type = "";

				}
				
				else if (regex_match(type, show_number)) {
					i++;
					while (line[i + 1] != '"') {
						temp.show_num += line[i];
						i++;
					}
					type = "";
				}

				else if (regex_match(type, value)) {
					i++;
					while (1) {
						if (line[i] == '"' && line[i + 1] == ',')
							break;
						else {
							temp.value += line[i];
							i++;
						}
					}

					if (temp.value == " ")
						temp.value = "$0";

					type = "";
				}

				else if (regex_match(type, air_date)) {
					i++;
					while (line[i + 1] != ',') {
						temp.air_date += line[i];
						i++;
					}
					type = "";

				}
				
				else if (regex_match(type, round_)) {
					i++;
					while (line[i + 1] != ',') {
						temp.round += line[i];
						i++;
					}
					type = "";
				}
			}
		
		//end of question
		if (line[i] == '}') {
			temp.question_num = total;//set question number
			list.push_back(temp);
			temp.init();// reset strings
			type = "";
			total++;
			if (total == amount)//stop whenn the number of questions specified by the user is reached
				return;
		}
	}
}

//returns string based on selected question data type
std::string getType(std::list<Questions>::iterator itr, int type) {

	switch (type) {

	case 1:
		return itr->category;
	case 2:
		return itr->value;
	case 3:
		return itr->question;
	case 4:
		return itr->answer;
	case 5:
		return itr->round;
	case 6:
		return itr->show_num;
	case 7:
		return itr->air_date;
	}
	return "";
}

//option 2
void filterList(std::list<Questions>& list, std::string filterInput, int type) {

	std::list<Questions> tempList;
	std::list<Questions>::iterator itr;
	std::string tempType = "";
	Questions insert;
	int count = 0;

	//look for EXACT match only for show numbers, this way if you seach for show number 1, you wont get 1000, 513, etc.
	if (type == 6) {

		for (itr = list.begin(); itr != list.end(); itr++) {
			tempType = itr->show_num;
			if (tempType == filterInput) {

				insert.category = itr->category;
				insert.value = itr->value;
				insert.question = itr->question;
				insert.answer = itr->answer;
				insert.round = itr->round;
				insert.show_num = itr->show_num;
				insert.air_date = itr->air_date;
				insert.question_num = itr->question_num;

				tempList.push_back(insert);
				count++;
			}
		}
	}else if (type == 2) {//value filter ignnores numerical characters before the filter string, so that if user searched for 200, they dont also get 1200 for example

		filterInput = "[^0-9]*" + filterInput;
		std::regex filter(filterInput);

		for (itr = list.begin(); itr != list.end(); itr++) {
			tempType = itr->value;
			
			if (regex_match(tempType, filter)) {

				insert.category = itr->category;
				insert.value = itr->value;
				insert.question = itr->question;
				insert.answer = itr->answer;
				insert.round = itr->round;
				insert.show_num = itr->show_num;
				insert.air_date = itr->air_date;
				insert.question_num = itr->question_num;

				tempList.push_back(insert);
				count++;
			}
		}
	}
	else {

		//trasform ecerything to lower case to compare, look for the filter input as a substring in any of the selected question data types
		transform(filterInput.begin(), filterInput.end(), filterInput.begin(), ::tolower);
		filterInput = ".*" + filterInput + ".*";
		std::regex filter(filterInput);

		for (itr = list.begin(); itr != list.end(); itr++) {
			tempType = getType(itr, type);
			transform(tempType.begin(), tempType.end(), tempType.begin(), ::tolower);
			if (regex_match(tempType, filter)) {

				insert.category = itr->category;
				insert.value = itr->value;
				insert.question = itr->question;
				insert.answer = itr->answer;
				insert.round = itr->round;
				insert.show_num = itr->show_num;
				insert.air_date = itr->air_date;
				insert.question_num = itr->question_num;

				tempList.push_back(insert);
				count++;
			}
		}
	}

		if (tempList.empty())
			std::cout << "could not find any questions matching the chosen filter\n\n";
		else {

			list = tempList;
			std::cout << "found and filtered" << count << " questions" << std::endl;
		}	
}

//option 5
void randomQuestion(std::list<Questions> list) {
	
	std::list<Questions>::iterator itr;
	int stop = rand() % list.size();//generate random number between 0 and list size

	//loop through list and decrement random number until 0 or end of list
	for (itr = list.begin(); itr != list.end(); itr++) {
		
		if (stop == 0)
			break;
		stop--;
	}

	std::cout << "CATEGORY: " << itr->category << std::endl;
	std::cout << "\nVALUE: " << itr->value << std::endl;
	std::cout << "\nQUESTION: " << itr->question << std::endl;

	//wait for user input to show anser
	std::cout << "\nPress Enter to see answer...\n";
	std::string temp;
	std::getline(std::cin, temp);
	std::cin.clear();
	std::cin.ignore(1000, '\n');

	std::cout << "ANSWER: " << itr->answer << std::endl;
}

std::list<Questions> search(std::list<Questions> list, std::string query){

	//IDF score is calculated by log(base 10) (M + 1)/k
	//where M = total number of questions in list and k is total number of questions containing the word W
	//it is then multiplied by the frequency of each word in every question then this score is summed for every word in the question

	//lots of lists since it needs to loop through 2 at a time for some parts
	std::list<Questions>::iterator itr, itr2;
	std::list<Words> WordList, WordList2, WordList3;//word list contains just the query words from the user, wordlist2 contains the frequency of each word for every question, and wordlist3 contains the query words and k (quesion frequency)
	std::list<Words>::iterator WordsItr, WordsItr2;
	std::list<Questions> QueryList, QueryList2; //list containing only questions that are relevant to query
	Words currentWord;
	std::string tempQuestion = "";//gets data from question and converts it to lower case for comparison
	Questions insert;

	//seperate words by space
	for (int i = 0; i <= query.length(); i++) {

		if (query[i] != ' ' && i != query.length())
			currentWord.word += tolower(query[i]);
		else {

			currentWord.word = ".*" + currentWord.word + " ?,?('?s)?";//optional space, comma, or 's

			WordList.push_back(currentWord);
			currentWord.word = "";
		}
	}

	int k = 0;//total number of questions containing word W
	int f = 0; // frequency of current word in in current question

	//loop through list of query words, find question frequency (k) and frequency of current word in a question
	for (WordsItr = WordList.begin(); WordsItr != WordList.end(); WordsItr++) {

		std::regex sub(WordsItr->word);
		k = 0;//reset question frequency for every new word
		for (itr = list.begin(); itr != list.end(); itr++) {

			tempQuestion = "";
			//check for match for every word in question
			for (int i = 0; i <= itr->question.length(); i++) {

				if (itr->question[i] != ' ' && i != itr->question.length()) {
					tempQuestion += tolower(itr->question[i]);
				}
				else if (regex_match(tempQuestion, sub)) {
					f++;
					tempQuestion = "";
				}else
					tempQuestion = "";
			}
			
			if (f > 0) {//if frequency is higher than 0, add it to another list to save its frequency 
				
				k++;
				currentWord.word = WordsItr->word;
				currentWord.f = f;
				currentWord.question_num = itr->question_num;
				WordList2.push_back(currentWord);
			}

			f = 0;
		}

		//insert k and current word into list after it is done searching for this word
		currentWord.word = WordsItr->word;
		currentWord.k = k;
		WordList3.push_back(currentWord);
	}

	float IDF;
	float M = list.size() + 1;//M + 1
	float dividend;
	float score;
	
	for (WordsItr = WordList3.begin(); WordsItr != WordList3.end(); WordsItr++) {
		
		dividend = M / WordsItr->k; // M + 1 / k
		
		IDF = log10(dividend);

		for (WordsItr2 = WordList2.begin(); WordsItr2 != WordList2.end(); WordsItr2++) {

			if (WordsItr->word == WordsItr2->word) {

				score = WordsItr2->f * IDF; //multiply each words frequency in the question by this value, it will later be summed up

				for (itr = list.begin(); itr != list.end(); itr++) {

					if (itr->question_num == WordsItr2->question_num) {
						
						insert.score = score;
						insert.category = itr->category;
						insert.value = itr->value;
						insert.question = itr->question;
						insert.answer = itr->answer;
						insert.round = itr->round;
						insert.show_num = itr->show_num;
						insert.air_date = itr->air_date;
						insert.question_num = itr->question_num;
						QueryList.push_back(insert);
				
						break;
					}
				}	
			}
		}
	}

	QueryList.sort(compareQuestionNum);//sort by question num to sum up the IDF scores easily

	itr = QueryList.begin();
	insert.question_num = itr->question_num;
	score = 0;
	
	for (itr = QueryList.begin(); itr != QueryList.end(); itr++) {

		if (insert.question_num == itr->question_num) {//this is for if a question contains multiple query words, then it will add them up

			score += itr->score;
			
		}
		else {
			insert.score = score;
		
			QueryList2.push_back(insert);

			insert.question_num = itr->question_num;
			score = itr->score;
		}

		insert.category = itr->category;
		insert.value = itr->value;
		insert.question = itr->question;
		insert.answer = itr->answer;
		insert.round = itr->round;
		insert.show_num = itr->show_num;
		insert.air_date = itr->air_date;
	}

	std::ofstream outfile;

	outfile.open("search_results.txt", std::ios::out | std::ios::trunc);

	QueryList2.sort(compareScore);//sort by decreasing score

	outfile << "Query used: " << query << std::endl << std::endl;

	for (itr = QueryList2.begin(); itr != QueryList2.end(); itr++) {
		
		outfile << "SCORE: " << itr->score << std::endl;
		outfile << "CATEGORY: " << itr->category << std::endl;
		outfile << "VALUE: " << itr->value << std::endl;
		outfile << "QUESTION: " << itr->question << std::endl;
		outfile << "ANSWER: " << itr->answer << std::endl;
		outfile << "ROUND: " << itr->round << std::endl;
		outfile << "SHOW NUM: " << itr->show_num << std::endl;
		outfile << "AIR DATE: " << itr->air_date << std::endl;
		outfile << std::endl;
	}
	outfile.close();
	if (QueryList2.empty())
		std::cout << "could not find any questions relevant to the query\n\n";
	else {
		std::cout << "found " << QueryList2.size() << " questions" << std::endl;
		return QueryList2;
	}

	return list;
}

int main() {

	//list = original non filtered questions, filtered list = filtered questions
	std::list<Questions> list, filteredList;
	std::list<Questions>::iterator itr;

	std::string line;

	std::fstream Datafile;
	std::ofstream DisplayFile;

	int amount = 0, option = 0, filterOption = 0, displayOption = 0;
	
	std::string filter = "", query = "";

	std::cout << "WELCOME TO THE JEOPARDY QUESTION DATA SET SORTER\n\n";
	std::cout << "How may questions would you like to take from the data set?\n";
	std::cout << "(Every 1500 questions takes around 1 second, 200,000+ questions available)\n\n";
	std::cout << "amount: ";

	while (!(std::cin >> amount)) {
		std::cout << "invalid amount\n\n";
		std::cin.clear();
		std::cin.ignore(1000, '\n');
		std::cout << "amount: ";
	}

	std::cout <<  "\nGetting " << amount << " questions from the data set. Please wait...\n";

	Datafile.open("JEOPARDY_QUESTIONS1.json", std::ios::in);

	getline(Datafile, line);
	getList(list, line, amount);

	Datafile.close();

	std::cout << "done!\n";

	filteredList = list;

	while (option != 7) {

		std::cout << std::endl;
		std::cout << "Which action would you like to preform?\n";
		std::cout << "1. Search, sort, and filter questions using a query (will also print results to 'search_results.txt')\n";
		std::cout << "2. Filter current list (ex. only filter questions with category = history)\n";
		std::cout << "3. Display all question data in current list (will also print results to 'display_results.txt')\n";
		std::cout << "4. Display specific question data in current list (will also print results to 'display_results.txt')\n";
		std::cout << "5. Play a random question from current list\n";
		std::cout << "6. Reset list (remove any filters applied)\n";
		std::cout << "7. QUIT\n";
		std::cout << std::endl;

		while (!(std::cin >> option) || option > 7 || option < 1) {
			std::cout << "invalid option\n\n";
			std::cin.clear();
			std::cin.ignore(1000, '\n');
		}
		std::cout << std::endl;

		switch (option) {

		//SEARCH USING QUERY
		case 1:
			
			std::cout << "\nEnter query (will use original list for search, any current filters will not apply): ";

			std::cin.clear();
			std::cin.ignore(1000, '\n');
			getline(std::cin, query);

			std::cout << "\nsearching list, please wait...\n";

			filteredList = search(list, query);
			
			break;
		//FILTER
		case 2:
			
			std::cout << "Which question data would you like to filter by?" << std::endl;
			std::cout << "1. CATEGORY (will look for categories containing a character sensitive match to the string)\n";
			std::cout << "2. VALUE (Please only enter numerical charcters. ex. 200)\n";
			std::cout << "3. QUSTION (will look for question containing a character sensitive match to the string)\n";
			std::cout << "4. ANSWER (will look for answers containing a character sensitive match to the string)\n";
			std::cout << "5. ROUND (Jeopardy!, Double Jeopardy!, Final Jeopardy! or Tiebreaker)\n";
			std::cout << "6. SHOW NUMBER (ex. 4680)\n";
			std::cout << "7. AIR DATE (YYYY-MM-DD)\n";
			std::cout << std::endl;

			while (!(std::cin >> filterOption) || filterOption > 7 || filterOption < 1) {
				std::cout << "invalid option\n\n";
				std::cin.clear();
				std::cin.ignore(1000, '\n');
			}

			std::cout << "\nEnter string to filter by: ";

			std::cin.clear();
			std::cin.ignore(1000, '\n');
			getline(std::cin, filter);

			std::cout << "\nfiltering list, please wait...\n";
			
			filterList(filteredList, filter, filterOption);

			break;

		//DISPLAY ALL QUESTION DATA
		case 3:
			DisplayFile.open("display_results.txt", std::ios::out | std::ios::trunc);
			for (itr = filteredList.begin(); itr != filteredList.end(); itr++) {
				DisplayFile << std::endl;
				DisplayFile << "CATEGORY: " << itr->category << std::endl;
				DisplayFile << "VALUE: " << itr->value << std::endl;
				DisplayFile << "QUESTION: " << itr->question << std::endl;
				DisplayFile << "ANSWER: " << itr->answer << std::endl;
				DisplayFile << "ROUND: " << itr->round << std::endl;
				DisplayFile << "SHOW NUM: " << itr->show_num << std::endl;
				DisplayFile << "AIR DATE: " << itr->air_date << std::endl;
				DisplayFile << std::endl;

				std::cout << std::endl;
				std::cout << "CATEGORY: " << itr->category << std::endl;
				std::cout << "VALUE: " << itr->value << std::endl;
				std::cout << "QUESTION: " << itr->question << std::endl;
				std::cout << "ANSWER: " << itr->answer << std::endl;
				std::cout << "ROUND: " << itr->round << std::endl;
				std::cout << "SHOW NUM: " << itr->show_num << std::endl;
				std::cout << "AIR DATE: " << itr->air_date << std::endl;
				std::cout << std::endl;
			}
			DisplayFile.close();
			break;
		//DISPLAY SPECIFIC QUESTION DATA
		case 4:

			std::cout << "Which question data would you like to display?\n";
			std::cout << "1. CATEGORY\n";
			std::cout << "2. VALUE\n";
			std::cout << "3. QUSTION\n";
			std::cout << "4. ANSWER\n";
			std::cout << "5. ROUND\n";
			std::cout << "6. SHOW NUMBER\n";
			std::cout << "7. AIR DATE\n";
			std::cout << std::endl;

			while (!(std::cin >> displayOption) || displayOption > 7 || displayOption < 1) {
				std::cout << "invalid option\n\n";
				std::cin.clear();
				std::cin.ignore(1000, '\n');
			}

			DisplayFile.open("display_results.txt", std::ios::out | std::ios::trunc);
			std::cout << std::endl;
			for (itr = filteredList.begin(); itr != filteredList.end(); itr++) {
				std::cout << getType(itr, displayOption) << std::endl;
				DisplayFile << getType(itr, displayOption) << std::endl;
			}

			DisplayFile.close();
			break;
		//RANDOM QUESTION AND ANSWER
		case 5:

			randomQuestion(filteredList);
			break;
		//REMOVE FILTERS
		case 6:

			filteredList = list;
			std::cout << "removed all filters\n";
			break;
		}
	}

	std::cout << "Goodbye!\n";
}