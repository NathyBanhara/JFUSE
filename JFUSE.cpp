#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include "json.hpp"
#include "graph.hpp"
#include "sax_event_consumer.hpp"
#include "grammar.hpp"
#include <queue>

using json = nlohmann::json;

int main(int argc, char *argv[])
{
    ios::sync_with_stdio(0); 
    cin.tie(0);

    string jsonFilename = argv[2];
    string fileGenerated = argv[3];
    
    std::ifstream text(jsonFilename);

    sax_event_consumer sec;

    bool result = json::sax_parse(text, &sec);
    
    std::cout << "\nresult: " << std::boolalpha << result << "\n" << flush;

    printf("\n");
    
    sec.graph.checkEnums();
    sec.graph.checkKeys(jsonFilename);

    sec.graph.getTaggedUnions(sec.queue.front()->name, sec.queue.front()->type, " ", sec.queue);
    sec.graph.checkTaggedUnions();

    sec.graph.percentualEdge();

    sec.graph.checkTupleAndCollection();

    sec.queue.freeQueue();
    cout << "\n\n";

    Grammar grammar = Grammar(sec.graph, fileGenerated);

    grammar.writeStructure("start_object.object", "", false, "", false);
    grammar.printAll();

    return 0;
}