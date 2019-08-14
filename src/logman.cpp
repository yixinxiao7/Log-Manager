// Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

//#include <getopt.h>
#include<iostream>
#include<fstream>
#include<vector>
#include<numeric>
#include<string>
#include<deque>
#include<string.h>
#include<algorithm>
#include<unordered_map>
#include<sstream>
#include "logman.h"

using namespace std;

//helper functions
void terminal_stuff(int argc, char *argv[]) {
	if (argc != 2) {
		cout << "you need more arguments\n";
		exit(0);
	}
	else if (argv[1][0] == '-') {	
		cout << "helpful message big yeet\n";
		exit(0);
	}
}

long long int convert_timestamp(string timestamp) {
	string newWord = "";
	newWord += timestamp[0];
	newWord += timestamp[1];
	newWord += timestamp[3];
	newWord += timestamp[4];
	newWord += timestamp[6];
	newWord += timestamp[7];
	newWord += timestamp[9];
	newWord += timestamp[10];
	newWord += timestamp[12];
	newWord += timestamp[13];
	return stoll(newWord);
}

int validate_timestamp(string timestamp) {
	if (timestamp.size() != 14) {
		cerr << "The timestamp(s) is not exactly 14 characters\n";
		return 1;
	}
	return 0;
}

int validate_entryID(int Size,int num) {
	if (num > Size - 1 ) {
		cerr << "The entryID is too large\n";
		return 1;
	}
	return 0;
}

void initialize_original_vector(const vector<LogEntry> &masterLog, vector<int> &original_vector ) {
	int ml_size = (int)masterLog.size();
	int entryID = 0;

	for (int i = 0; i < ml_size; ++i) {
		entryID = masterLog[i].EntryID;
		original_vector[entryID] = i;
	}
}

vector<string> convert_lower_case(string originalWord) {
	vector<string>lower_case_vec;
	int b = 0;
	int e = 0;
	bool need_to_find_new_word = true;
	string substring = "";
	int size = (int)originalWord.size();
	for (int i = 0; i < size; ++i) {
		if (isalnum(originalWord[i])) {
			if (need_to_find_new_word) {
				b = i;
				e = i + 1;
			}
			else {
				++e;
			}
			need_to_find_new_word = false;
		}
		else {//e is at a non-alphanumeric character. word is found. OR start of new word has not been found
			if (!need_to_find_new_word) {
				substring = originalWord.substr(b, e - b);
				std::transform(substring.begin(), substring.end(), substring.begin(), ::tolower);
				lower_case_vec.push_back(substring);
				b = e;
				need_to_find_new_word = true;
			}
		}
	}//for
	if (!need_to_find_new_word) {
		substring = originalWord.substr(b, e - b);
		std::transform(substring.begin(), substring.end(), substring.begin(), ::tolower);
		lower_case_vec.push_back(substring);
	}
	return lower_case_vec;
}

//initializer functions 

void initialize_cat_map(const vector<LogEntry> &masterLog, unordered_map<string, vector<int> > &catMap) {
	string next_cat = "";
	int most_recently_added = -1;
	int masterLogSize = (int)masterLog.size();
	for (int i = 0; i < masterLogSize; ++i) {
		next_cat = masterLog[i].cat;
		std::transform(next_cat.begin(), next_cat.end(), next_cat.begin(), ::tolower);

		auto it = catMap.find(next_cat);//string iterator
		if (it == catMap.end()) {//doesn't exist yet
			vector<int> cat;
			cat.push_back(i);//vector of indices
			catMap[next_cat] = cat;//catmap[key] = value
		}
		else {//found it, add entry
			most_recently_added = it->second[it->second.size() - 1];
			if (i != most_recently_added) {
				it->second.push_back(i);
			}
		}
	}
}

//should be done
void initialize_keyword_map(const vector<LogEntry> &masterLog, unordered_map<string, vector<int> > &keyWordMap) {
	string next_cat = "";
	string key_word_cat = "";
	int most_recently_added = -1;
	int master_size = (int)masterLog.size();
	for (int i = 0; i < master_size; ++i) {
		next_cat = masterLog[i].cat;
		vector<string> transformed_words = convert_lower_case(next_cat);
		int size = (int) transformed_words.size();
		for (int j = 0; j < size; ++j) {
			key_word_cat = transformed_words[j];
			auto it = keyWordMap.find(key_word_cat);
			if (it == keyWordMap.end()) {
				vector<int> entriesWithWord;
				entriesWithWord.push_back(i);
				keyWordMap[key_word_cat] = entriesWithWord;
			}
			else {
				most_recently_added = it->second[it->second.size() - 1];
				if (i != most_recently_added) {
					it->second.push_back(i);
				}
			}
		}
		//handle message
		string message = masterLog[i].message;
		string keyword = "";
		vector<string> transformed_message_words = convert_lower_case(message);
		int message_size = (int)transformed_message_words.size();
		for (int k = 0; k < message_size; ++k) {
			keyword = transformed_message_words[k];
			auto it = keyWordMap.find(keyword);
			if (it == keyWordMap.end()) {
				vector<int> entriesWithWord;
				entriesWithWord.push_back(i);
				keyWordMap[keyword] = entriesWithWord;
			}
			else {
				most_recently_added = it->second[it->second.size() - 1];
				if (i != most_recently_added) {
					it->second.push_back(i);
				}
			}
		}
	}
}
//User commands

//should be done
void timestamp_search(const vector<LogEntry> &masterLog, char &most_recent_search, vector<LogEntry>::const_iterator &low, vector<LogEntry>::const_iterator &up) {
	string ts1;
	cin >> ts1;
	//check if proper timestamp
	int eject = validate_timestamp(ts1);
	if (eject == 1) {
		return;
	}
	long long int ts1_i;
	ts1_i = convert_timestamp(ts1);
	low = lower_bound(masterLog.begin(), masterLog.end(), ts1_i, compareTSLOWER());
	up = upper_bound(masterLog.begin(), masterLog.end(), ts1_i, compareTSUPPER()); //includes one number too much/high
	most_recent_search = 't';
	if (low == masterLog.end()) {//element not found
		cout << "Timestamp search: 0 entries found\n";
		return;//preserve last search results
	}
	cout << "Timestamp search: " << up - low << " entries found\n";
	//save this search results
}

//should be done
void timestamps_search(const vector<LogEntry> &masterLog, char &most_recent_search, vector<LogEntry>::const_iterator &low, vector<LogEntry>::const_iterator &up) {
	string ts1, ts2;
	getline(cin, ts1, '|');
	cin >> ts2;
	ts1 = ts1.substr(1);
	int eject = validate_timestamp(ts1);
	if (eject == 1) {
		return;
	}
	eject = validate_timestamp(ts2);
	if (eject == 1) {
		return;
	}
	long long ts1_int = convert_timestamp(ts1);
	long long ts2_int = convert_timestamp(ts2);
	low = lower_bound(masterLog.begin(), masterLog.end(), ts1_int, compareTSLOWER());
	up = upper_bound(masterLog.begin(), masterLog.end(), ts2_int, compareTSUPPER()); 

	most_recent_search = 't';

	if(low == masterLog.end()){//if nothing found, exit
		cout << "Timestamps search: 0 entries found\n";
		return;//preserve last search results
	}
	cout << "Timestamps search: " << up - low << " entries found\n";
}

void category_search(const unordered_map<string, vector<int> > &catMap, char &most_recent_search, string &recent_word) {
	string category;
	getline(cin, category);
	category = category.substr(1);
	//if running on visual debugger OR AUTOGRADER, comment this out.
	//category.erase(category.size() - 1);
	std::transform(category.begin(), category.end(), category.begin(), ::tolower);
	auto it = catMap.find(category);
	most_recent_search = 'c';

	if (it == catMap.end()) {
		cout << "Category search: 0 entries found\n";
		recent_word = "none";
		return;
	}
	else {//found the key category. so now we have a sorted vector of all the masterID entries with that cat
		cout << "Category search: " << it->second.size() << " entries found\n";
		recent_word = it->first;
	}
}

//should be done 
void keyword_search(const unordered_map<string, vector<int> > &keyWordMap, char &most_recent_search, vector<int> &recent_keyword) {
	string search_word = "";
	string line = "";
	getline(cin, line);
	line = line.substr(1);
	//vector<int> vec;
	vector<int> vec2;
	vector<string> transformed_words = convert_lower_case(line);
	int size = (int)transformed_words.size();
	most_recent_search = 'k';
	while (!recent_keyword.empty()) {
		recent_keyword.pop_back();
	}
	//end program early as long as one the key words doesnt exist
	for (int i = 0; i < size; ++i) {
		search_word = transformed_words[i];
		auto it = keyWordMap.find(search_word);
		if (it == keyWordMap.end()) {
			cout << "Keyword search: 0 entries found\n";
			return;
		}
	}

	//all keywords exist now
	for (int i = 0; i < size; ++i) {
		search_word = transformed_words[i];
		auto it = keyWordMap.find(search_word);
		if (recent_keyword.empty()) {
			recent_keyword = it->second;
		}
		else {
			vec2 = it->second;
		}
		
		if (!recent_keyword.empty() && !vec2.empty()) {
			auto it = set_intersection(recent_keyword.begin(), recent_keyword.end(), vec2.begin(), vec2.end(), recent_keyword.begin());
			recent_keyword.resize(it - recent_keyword.begin());
			if (recent_keyword.empty()) {
				cout << "Keyword search: 0 entries found\n";
				return;
			}
			while (!vec2.empty()) {
				vec2.pop_back();
			}
		}
	}

	cout << "Keyword search: " << recent_keyword.size() << " entries found\n";
}

//helper function

void append(deque<int> &EL, const vector<int> &original_vector, int masterLogSize) {
	int num;
	cin >> num;
	int eject = validate_entryID((int)masterLogSize, num);
	if (eject == 1) {
		return;
	}
	cout << "log entry " << num << " appended\n";
	num = original_vector[num];
	EL.push_back(num);
}

//no need for masterLogIndices
void append_search(deque<int> &EL, const vector<LogEntry> &masterLog, char most_recent_search, vector<LogEntry>::const_iterator &low, vector<LogEntry>::const_iterator &up, string cat, unordered_map<string, vector<int> > &catMap, vector<int> &recent_keyword) {
	if (most_recent_search == '*') {
		cerr << "A previous search has to have occurred\n";
		return;
	}
	else if (most_recent_search == 't') {
		int i = (int)(low - masterLog.begin());
		int counter = 0;
		auto temp = low;
		while (temp < up) {
			EL.push_back(i);
			++temp;
			++i;
			++counter;
		}
		cout << counter << " log entries appended\n";
	}
	else if (most_recent_search == 'c') {
		if (cat != "none") {
			auto it = catMap.find(cat);
			int vecsize = (int)it->second.size();
			int counter1 = 0;
			for (int i = 0; i < vecsize; ++i) {
				EL.push_back(it->second[i]);
				++counter1;
			}
			cout << counter1 << " log entries appended\n";
		}
		else {
			cout << "0 log entries appended\n";
		}
	}
	else if (most_recent_search == 'k'){
		int ksize = (int)recent_keyword.size();
		int counter2 = 0;
		for (int i = 0; i < ksize; ++i) {
			EL.push_back(recent_keyword[i]);
			++counter2;
		}
		cout << counter2 << " log entries appended\n";
	}
}

//should be optimized
void delete_entry(deque<int> &EL) {
	int num;
	cin >> num;
	int eject = validate_entryID((int)EL.size(), num);
	if (eject == 1) {
		return;
	}
	EL.erase(EL.begin() + num);
	cout << "Deleted excerpt list entry " << num << '\n';
}

//O(n) Optimized
void move_to_beginning(deque<int> &EL) {
	int num;
	//int counter = 0;
	cin >> num;
	int eject = validate_entryID((int) EL.size(), num);
	if (eject == 1) {
		return;
	}

	int temp = EL[num];
	EL.erase(EL.begin() + num);
	EL.push_front(temp);
	cout << "Moved excerpt list entry " << num << '\n';
}

//O(n). Optimized
void move_to_end(deque<int> &EL) {
	int num;
	//int counter = 0;
	cin >> num;//excerpt entry list
	int eject = validate_entryID((int) EL.size(), num);
	if (eject == 1) {
		return;
	}

	int temp = EL[num];
	EL.erase(EL.begin() + num);
	EL.push_back(temp);
	cout << "Moved excerpt list entry " << num << '\n';
}

//relook at this one
void sort_EL( vector<LogEntry> &masterLog,  deque<int> &EL) {
	cout << "excerpt list sorted\n";
	if (EL.empty()) {
		cout << "(previously empty)\n";
		return;
	}
	int lastelt = (int) (EL.size() - 1);
	cout << "previous ordering:\n";
	cout << 0 << '|' << masterLog[EL[0]].EntryID << '|' << masterLog[EL[0]].timeStampStr
		<< '|' << masterLog[EL[0]].cat << '|' << masterLog[EL[0]].message << '\n' ;
	cout << "...\n";
	cout << lastelt << '|' << masterLog[EL[lastelt]].EntryID << '|' << masterLog[EL[lastelt]].timeStampStr
		<< '|' << masterLog[EL[lastelt]].cat << '|' << masterLog[EL[lastelt]].message << '\n';

	std::sort(EL.begin(), EL.end());

	cout << "new ordering:\n";
	cout << 0 << '|' << masterLog[EL[0]].EntryID << '|' <<masterLog[EL[0]].timeStampStr
		<< '|' << masterLog[EL[0]].cat << '|' << masterLog[EL[0]].message << '\n';
	cout << "...\n";
	cout << lastelt << '|' << masterLog[EL[lastelt]].EntryID << '|' << masterLog[EL[lastelt]].timeStampStr
		<< '|' << masterLog[EL[lastelt]].cat << '|' << masterLog[EL[lastelt]].message << '\n';

}

void clear(const vector<LogEntry> &masterLog, deque<int> &EL) {
	//print EL
	cout << "excerpt list cleared\n";
	if (EL.empty()) {
		cout << "(previously empty)\n";
		return;
	}
	int lastelt = (int)(EL.size() - 1);
	cout << "previous contents:\n";
	cout << 0 << '|' << masterLog[EL[0]].EntryID << '|' << masterLog[EL[0]].timeStampStr
		<< '|' << masterLog[EL[0]].cat << '|' << masterLog[EL[0]].message << '\n';
	cout << "...\n";
	cout << lastelt << '|' << masterLog[EL[lastelt]].EntryID << '|' << masterLog[EL[lastelt]].timeStampStr
		<< '|' << masterLog[EL[lastelt]].cat << '|' << masterLog[EL[lastelt]].message << '\n';
	while (!EL.empty()) {
		EL.pop_back();
	}
}

//should be done now
void print_recent_search(const vector<LogEntry> &masterLog, char most_recent_search, vector<LogEntry>::const_iterator &low, vector<LogEntry>::const_iterator &up, string cat, unordered_map<string, vector<int> > &catMap, vector<int> &recent_keyword) {
	if (most_recent_search == '*') {
		cerr << "No previous searches have occured yet\n";
		return;
	}
	else if (most_recent_search == 't') {
		int i = (int)(low - masterLog.begin());
		auto temp = low;
		while (temp < up) {
			cout << masterLog[i].EntryID << '|'
				<< masterLog[i].timeStampStr << '|' << masterLog[i].cat
				<< '|' << masterLog[i].message << '\n';
			++temp;
			++i;
		}
	}
	else if (most_recent_search == 'c'){
		if (cat != "none") {
			auto it = catMap.find(cat);
			int vecsize = (int)it->second.size();
			for (int i = 0; i < vecsize; ++i) {
				cout << masterLog[it->second[i]].EntryID << '|'
					<< masterLog[it->second[i]].timeStampStr << '|' << masterLog[it->second[i]].cat
					<< '|' << masterLog[it->second[i]].message << '\n';
			}
		}
	}
	else if (most_recent_search == 'k') {
		int ksize = (int)recent_keyword.size();
		for (int i = 0; i < ksize; ++i) {
			cout << masterLog[recent_keyword[i]].EntryID << '|'
				<< masterLog[recent_keyword[i]].timeStampStr << '|' << masterLog[recent_keyword[i]].cat
				<< '|' << masterLog[recent_keyword[i]].message << '\n';
		}
	}
}

//check
void print(const vector<LogEntry> &masterLog, const deque<int> &EL) {
	for (size_t i = 0; i < EL.size(); ++i) {
		cout << i << '|' << masterLog[EL[i]].EntryID << '|'
			<< masterLog[EL[i]].timeStampStr << '|' << masterLog[EL[i]].cat
			<< '|' << masterLog[EL[i]].message << '\n';
	}
}

//main program
//consider putting this straight into main so you don't have to pass in so many parameters
void command(char cmd, vector<LogEntry> &masterLog, const vector<int> &original_vector, deque<int> &EL, unordered_map<string, vector<int> > &catMap, const unordered_map<string, vector<int> >  &keyWordMap, char &most_recent_search, string &cat, vector<int> &recent_keyword, vector<LogEntry>::const_iterator &low, vector<LogEntry>::const_iterator &up) {//consider a switch statement
	if (cmd == 't') {
		timestamps_search(masterLog, most_recent_search, low, up);
	}
	else if (cmd == 'm') {
		timestamp_search(masterLog, most_recent_search, low, up);
	}
	else if (cmd == 'c') {
		category_search(catMap, most_recent_search, cat);
	}
	else if (cmd == 'k') {
		keyword_search(keyWordMap, most_recent_search, recent_keyword);
	}
	else if (cmd == 'a') {
		append(EL, original_vector, (int)(masterLog.size()));
	}
	else if (cmd == 'r') {
		append_search(EL, masterLog, most_recent_search, low, up, cat, catMap, recent_keyword);
	}
	else if (cmd == 'd') {
		delete_entry(EL);
	}
	else if (cmd == 'b') {
		move_to_beginning(EL);
	}
	else if (cmd == 'e') {
		move_to_end(EL);
	}
	else if (cmd == 's') {
		sort_EL(masterLog, EL);
	}
	else if (cmd == 'l') {
		clear(masterLog, EL);
		return;
	}
	else if (cmd == 'g') {
		print_recent_search(masterLog, most_recent_search, low, up, cat, catMap, recent_keyword);
	}
	else if (cmd == 'p') {
		print(masterLog, EL);
		return;
	}
	else if (cmd == '#') {
		string garbage = "";
		getline(cin, garbage);
	}
	else if(cmd != 'q') {
		cerr << "That's an improper command\n";
	}
}

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);

	terminal_stuff(argc, argv);

	//arv[1] is not help
	string file_name = argv[1];

	ifstream is(file_name);
	if (!is.is_open()) {
		cout << "file could not be found\n";
		exit(0);
	}

	//read in master file
	vector<LogEntry> masterLog;

	string timestamp, cat, mes;
	LogEntry logentry;
	int entryID = 0;
	long long int converted_ts = 0;
	while (getline(is, timestamp, '|')) {//read in timestamp
		getline(is, cat, '|');//read in category
		getline(is, mes);//read in mes
		converted_ts = convert_timestamp(timestamp);
		logentry = {timestamp, cat, mes, converted_ts, entryID };
		masterLog.push_back(logentry);
		++entryID;
	}
	cout << entryID << " entries read\n";
	
	//sort masterLog's INDICES in order
	sort(masterLog.begin(), masterLog.end(), sortMasterLog());

	//maintain masterlog's old order
	vector<int> original_vector;
	original_vector.resize((int)masterLog.size());
	initialize_original_vector(masterLog, original_vector);

	//iterators for timestamp searching
	vector<LogEntry>::const_iterator low;
	vector<LogEntry>::const_iterator up;
	//map for cat searaching
	unordered_map<string, vector<int> > catMap;
	initialize_cat_map(masterLog, catMap);
	//map for keyword searching
	unordered_map<string, vector<int> > keyWordMap;
	initialize_keyword_map(masterLog, keyWordMap);
	vector<int> recent_keyword;
	//excerpt list declaration
	deque<int> EL;
	
	//to check if a search appears
	char most_recent_search = '*';
	//user commands
	char cmd;
	do {
		cout << "% ";
		cin >> cmd;

		command(cmd, masterLog, original_vector, EL, catMap, keyWordMap, most_recent_search, cat, recent_keyword, low, up);
	} while (cmd != 'q');
	cout << '\n';
}