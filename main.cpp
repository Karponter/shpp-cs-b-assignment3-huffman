#include "typedef.h"
#include "nodestruct.h"
#include "RevPrioQueue.h"

using namespace std;

void compress_file(string if_name, string of_name) {

	// creating a frequency map
	FreqMap *input_frequency_map = new FreqMap();
	{
		// reading an input file and filling frequencies to a map
		ifstream ifs(if_name, ios::binary);
	    char buffer;
	    while (ifs.get(buffer))
	    	(*input_frequency_map)[buffer]++;
	    ifs.close();
	}

	// creating nodes priority queue
	// filling it with new unlinked nodes
	RevPrioQueue* free_nodes = new RevPrioQueue();
	{
		int n = 0;
		auto FQM_it = input_frequency_map->begin();
		auto FQM_end_it = input_frequency_map->end();
		while (FQM_it != FQM_end_it) {
			n++;
			free_nodes->enqueue(new Node(FQM_it->first, FQM_it->second));
			FQM_it++;
		}
		cout << n << endl;
	}
	
	// creating Huffman tree
	while (free_nodes->size() > 1) {
		auto node1 = free_nodes->dequeue();
		auto node2 = free_nodes->dequeue();
		free_nodes->enqueue(new Node(node1, node2));
	}
	Node* huffman_coding_tree = free_nodes->dequeue();
	auto huffman_coding_map = huffman_coding_tree->makeMap();

	// free useless allocated data
	delete huffman_coding_tree;
	delete input_frequency_map;
	delete free_nodes;

	// read input file and compress it
    vector<bool> *buffer = new vector<bool>();
	{
	    ifstream iFS(if_name, ios::binary);
	    char temp_ch;
	    while (iFS.get(temp_ch)) {
	        vector<bool> code = (*huffman_coding_map)[temp_ch];
	       	buffer->insert(buffer->end(), code.begin(), code.end());
	    }
	    iFS.close();
	}{
	    // push 0 bits to the end of buffer, to gain data to be bytewise
	    int bufferSize = buffer->size();
	    while (buffer->size() % 8 != 0)
	        buffer->push_back(0);

	    ofstream oFS(of_name, ios::binary);

	    // writing to output file numbet of Huffman codes
	    unsigned int HCM_length = huffman_coding_map->size();
	    cout << HCM_length << endl;
	    oFS.write((char*) &HCM_length, sizeof(int));

	    // writing to output file Huffman coding map
	    // chunk size is 6 bytes (for 4-byte integer systems)
	    // 1 -- character key (raw file data piece)
	    // 2 -- length of Huffman code
	    // 3-6 -- Huffman code with appended zeros for a valid chunk size
	    for (huffmanMap::iterator HCM_it = huffman_coding_map->begin(); HCM_it != huffman_coding_map->end(); HCM_it++) {
	        
	        oFS.put((*HCM_it).first);
	        vector<bool> code = (*HCM_it).second;
	        oFS.put(code.size());

	        // appending 0 bits
	        while (code.size() != sizeof(int) * 8)
	            code.push_back(0);

	        unsigned int huffCode = 0;
	        for (vector<bool>::iterator HC_it = code.begin(); HC_it != code.end(); HC_it++)
	        	huffCode = (huffCode << 1) | *HC_it;

	        oFS.write((char*) &huffCode, sizeof(int));
	    }

	    // writing to output file stored buffer size
	    oFS.write((char*) &bufferSize, sizeof(int));

	    // writing to output file buffer with coded raw chunks
	    // chunk size is 1 byte
	    int chunk_filling = 0;
	    char chunk = 0;
	    for (vector<bool>::iterator BUF_it = buffer->begin(); BUF_it != buffer->end(); BUF_it++) {
	        chunk = (chunk << 1) | *BUF_it;
	        chunk_filling++;
	        if ((chunk_filling % 8) == 0) {
	            oFS.put(chunk);
	            chunk_filling = 0;
	            chunk = 0;
	        }
	    }
	    oFS.close();
	}

	// free useless allocated data
	delete huffman_coding_map;
	delete buffer;
}

void decompress_file(string if_name, string of_name) {

	// reading compressed file and restoring Huffman conding map container
    reverseHuffmanMap* huffman_decoding_map = new reverseHuffmanMap();
    vector<bool>* buffer = new vector<bool>();
    {
	    ifstream iFS(if_name, ios::binary);
	    {
		    // reading stored Huffman map size
		    unsigned int tableSize = 0;
		    iFS.read((char*) &tableSize, sizeof(int));
		    cout << (int)tableSize << endl;

		    // reading Huffman coding map
		    // chunk size is 6 bytes (for 4-byte integer systems)
		    // 1 -- character key (raw file data piece)
		    // 2 -- length of Huffman code
		    // 3-6 -- Huffman code with appended zeros for a valid chunk size
		    for (int i = 0; i < tableSize; i++) {

		        char data_chunk;
		        iFS.get(data_chunk);
		        char code_length;
		        iFS.get(code_length);

		        unsigned int huffCode = 0;
		        iFS.read((char*) &huffCode, sizeof(int));

		        // creating trimmed Huffman with original size
		        vector<bool> code;
		        for (int j = 0; j < code_length; j++) {
		            code.push_back(huffCode & 0b10000000000000000000000000000000);
		            huffCode <<= 1;
		        }

		    	// storing loaded data to Huffman coding map container
		    	(*huffman_decoding_map)[code] = data_chunk;
		    }
	    }

	    // reading original coded data size for trimming
	    unsigned int bufferSize = 0;
	    iFS.read((char*) &bufferSize, sizeof(int));

	    // reading coded data from compressed file
	    char byte = 0;
	    while (iFS.get(byte))
	        for (int i = 0; i < 8; i++) {
	            buffer->push_back(byte & 0b10000000);
	            byte <<= 1;
	        }
	    
	    // trimming coded data to original size
	    while(buffer->size() != bufferSize)
	        buffer->pop_back();

	    iFS.close();
	}

	// decompressing coded data
	// using Huffman coding table to restore raw data from code 
	{
	    ofstream oFS(of_name, ios::binary);
	    vector<bool> code;
	    for (vector<bool>::iterator it = buffer->begin(); it != buffer->end(); it++){
	        code.push_back(*it);
	        if (huffman_decoding_map->count(code)) {
	            oFS.put((*huffman_decoding_map)[code]);
	            code.clear();
	        }
	    }
	}

	// free useless allocated data
	delete huffman_decoding_map;
	delete buffer;
}

int main(int argc, char const *argv[]) {
	compress_file("res/test.png", "res/compressed");
	decompress_file("res/compressed", "res/otp.png");
	return 0;
}