#define FILE_EXTENSION ".txt"
#include <bits/stdc++.h>

using namespace std;

vector<string> word_parse(vector<string> tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch))
				new_str.push_back(tolower(ch));
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> q_word_parse(vector<string> tmp_string){
	vector<string> parse_string;
	for(auto& word : tmp_string){
		string new_str;
    	for(auto &ch : word){
			if(isalpha(ch) || ch=='+' || ch=='/' || ch=='-' || ch=='*' || ch=='<' || ch=='>' || ch=='\"')
				new_str.push_back(tolower(ch));
		}
		parse_string.emplace_back(new_str);
	}
	return parse_string;
}

vector<string> split(const string& str, const string& delim) {
	vector<string> res;
	if("" == str) return res;

	char * strs = new char[str.length() + 1] ; 
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while(p) {
		string s = p; 
		res.push_back(s); 
		p = strtok(NULL, d);
	}
	return res;
}


struct TrieNode {
    bool isWord;
	int id;
	TrieNode* chil[26] = {NULL};
	TrieNode* parent = NULL;

    TrieNode(bool iW, int i): isWord(iW), id(i) {}

};

vector<TrieNode*> tries;
vector<TrieNode*> leaf[100005];

TrieNode* createTrieNode(char c) {
    return new TrieNode(0, c-'a');
}


void insert(TrieNode* root, const std::string& word, int i) {
    if (word.empty()) return;

    TrieNode* node = root;
    for (char c : word) {
		if (c < 'a' || c > 'z') continue;
        if (node->chil[c - 'a'] == NULL) {
            node->chil[c - 'a'] = createTrieNode(c);
            node->chil[c - 'a']->parent = node;
        }
        node = node->chil[c - 'a'];
    }
    node->isWord = true;
	leaf[i].push_back(node);
}


bool exact_search(TrieNode* root, const std::string& word) {
    TrieNode* node = root;
    for (char c : word) {
        if (node == NULL || node->chil[c - 'a'] == NULL) {
            return false;
        }
        node = node->chil[c - 'a'];
    }
    return node->isWord;
}

bool prefix_search(TrieNode* root, const std::string& prefix) {
    TrieNode* node = root;
    for (char c : prefix) {
        if (node == NULL || node->chil[c - 'a'] == NULL) {
            return false;
        }
        node = node->chil[c - 'a'];
    }
    return true;
}

bool suffix_search(TrieNode* root, const std::string& word, int docIndex) {
	string reversed_word(word.rbegin(), word.rend());

	 for (TrieNode* leaf_node : leaf[docIndex]) {
        TrieNode* node = leaf_node;
        int i = 0;

        while (node != NULL && i < reversed_word.size()) {
            if (node->id != reversed_word[i] - 'a')  break;
            node = node->parent;
            i++;
        }
        if (i == reversed_word.size()) {
            return true;
        }
    }
    return false;
}


bool wildcard_search_helper(TrieNode* node, const std::string& word, int word_idx) {
    if (word_idx == word.size()) return node->isWord;

    if (word[word_idx] == '*') {
        // '*' matches zero character
        if (wildcard_search_helper(node, word, word_idx + 1)) {
            return true;
        }

        // '*' matches one or more characters
        for (TrieNode* child : node->chil) {
            if (child != NULL && wildcard_search_helper(child, word, word_idx)) {
                return true;
            }
        }
    } else {
        int childIndex = word[word_idx] - 'a';
        if (node->chil[childIndex] != NULL && wildcard_search_helper(node->chil[childIndex], word, word_idx + 1)) {
            return true;
        }
    }

    return false;
}

bool wildcard_search(TrieNode* root, const std::string& word) {
    return wildcard_search_helper(root, word, 0);
}


bool search_mode(TrieNode* root, const std::string& word, int idx) {
	if (word.empty() || root == NULL)  return false;

	if(word[0] == '\"' && word[word.size()-1] == '\"'){
		string n_word = word.substr(1, word.size()-2);
		return exact_search(root, n_word);
	}
	else if(word[0] == '*' && word[word.size()-1] == '*'){
		string n_word = word.substr(1, word.size()-2);
		return suffix_search(root, n_word, idx);
	}
	else if(word[0] == '<' && word[word.size()-1] == '>'){
		string n_word = word.substr(1, word.size()-2);
		return wildcard_search(root, n_word);
	}
	else return prefix_search(root, word);
}


int main(int argc, char *argv[]){

    string data_dir = argv[1] + string("/");
	string query = string(argv[2]);
	string output = string(argv[3]);

	string file;
	string title_line, content_line, q_line;
	vector<string> title_split, content_split, q_split;
	vector<string> title, content, q;
	vector<string> title_for_output;
	
	int oper;
	bool tmp_ans, ans, exist_ans;

	//1. txt & build trie

	int i=0;
	while(1){
			
		TrieNode* root = createTrieNode('\0');
		
		//1) open file from data/0.txt to data/100.txt
		file = data_dir + to_string(i) + ".txt";
		fstream fi(file, ios::in);

		if(!fi.is_open()) break;
		
		//2) get title
		getline(fi, title_line);
		title_split = split(title_line, " ");
		title = word_parse(title_split);
		title_for_output.push_back(title_line); //special store title for output

		for(auto &word : title){
			insert(root,word, i);
		}

		//3) get content line by line
		while(getline(fi, content_line)){
			content_split = split(content_line, " ");
			content = word_parse(content_split);
			for(auto &word : content){
				insert(root,word, i);
			}
		}

		//4) push root to tries
		tries.push_back(root);

		//5) close 0.txt to 100.txt
		fi.close();
		
		i++;
	}


	//2. query & search & output

    ifstream q_fi(query);
    ofstream outputFile(output);

	// for each line of query
	while(getline(q_fi, q_line)){
		exist_ans = 0;

		// for each trie
		for(size_t idx=0; idx<tries.size(); idx++){
			TrieNode* root = tries[idx];
			oper = 0;
			tmp_ans = true;
			ans  = true;

			// for each word in each line of query
			q_split = split(q_line, " ");
			q = q_word_parse(q_split);

			// search
			for(auto &q_word : q){

				if((q_word != "+") && (q_word != "/") && (q_word != "-")){
					if(oper == 1) ans = tmp_ans & search_mode(root, q_word, idx);
					else if(oper == 2) ans = tmp_ans | search_mode(root, q_word, idx);
					else if(oper == 3) ans = tmp_ans & !search_mode(root, q_word, idx);
					else ans = search_mode(root, q_word, idx);
					tmp_ans = ans;	
				}

				if(q_word == "+") oper = 1;
				else if(q_word == "/") oper = 2;
				else if(q_word == "-") oper = 3;
				else oper = 0;

			}
			if(ans){
				//outputFile << q_line << " "  << idx << " " << title_for_output[idx] << endl;
				outputFile << title_for_output[idx] << endl;
				exist_ans = 1;
			}
		}
		if(!exist_ans) outputFile << "Not Found!" << endl;
	}

	// close query.txt
	q_fi.close();
    outputFile.close();

}
