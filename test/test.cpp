
#include <protobuf2rapidjson.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <google/protobuf/text_format.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/filestream.h>
#include <rapidjson/stringbuffer.h>
#include "test.pb.h"

using namespace std;
using namespace Protobuf2Rapidjson;
using namespace rapidjson;
using namespace google::protobuf;

int main(int argc, char *argv[]) {
	
	FILE *test = fopen("test.json","r");
	FileStream test_stream(test);

	Document document;
	document.ParseStream<0>(test_stream);
	
	if(document.HasParseError()) {
		cout << "Parse error!" << endl;
		return -1;
	}

	addressbook::Person person;
	if(!decode_from_json_value(document, &person)) {
		cout << "Decoding error!" << endl;
		return -1;
	}

	string text;
	TextFormat::PrintToString(person, &text);
	cout << "----------------------------" << endl;
	cout << "PROTBUF TEXT:" << endl;
	cout << text << endl;

	Document encoded;
	//rj::Document::AllocatorType& alloc = reply.GetAllocator();
	encode_to_json_doc(person, &encoded);
	StringBuffer output;
	PrettyWriter<StringBuffer> writer(output);
	encoded.Accept(writer);
	output.Put('\n');

	cout << "----------------------------" << endl;
	cout << "BACK TO JSON:" << endl;
	cout << string(output.GetString(), output.GetString()+output.Size()) << endl;

	//test error reporting
	string err;

	cout << "----------------------------" << endl;
	cout << "TESTING ERROR MESSAGES:" << endl;

	document["phone"][1]["type"].SetString("error?!");
	if(!decode_from_json_value(document, &person, &err))
		cout << "Message: " << err << endl;

	document["phone"][1].SetObject();
	Value b("i am not here");
	document["phone"][1].AddMember("not_existing", document.GetAllocator(), b, document.GetAllocator());
	if(!decode_from_json_value(document, &person, &err))
		cout << "Message: " << err << endl;

	document["phone"][1].SetDouble(5);
	if(!decode_from_json_value(document, &person, &err))
		cout << "Message: " << err << endl;

	document["phone"].SetDouble(5);
	if(!decode_from_json_value(document, &person, &err))
		cout << "Message: " << err << endl;

	return 0;
}
