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

int main()
{
    ios::sync_with_stdio(0); 
    cin.tie(0);
    
    //std::ifstream text("extractTwitter.json");
    std::ifstream text("extractPrescrip.json");
    //std::ifstream text("data.json");
    //std::ifstream text("doc.json");

    sax_event_consumer sec;

    bool result = json::sax_parse(text, &sec);
    
    std::cout << "\nresult: " << std::boolalpha << result << "\n" << flush;

    printf("\n");
    
    sec.graph.checkEnums();

    sec.graph.getTaggedUnions(sec.queue.front()->name, sec.queue.front()->type, " ", sec.queue);

    sec.graph.percentualEdge();

    sec.graph.checkTupleAndCollection();

    sec.queue.freeQueue();
    cout << "\n\n";

    Grammar grammar = Grammar(sec.graph);

    grammar.writeStructure("start_object.object", "", false, "", false);
    grammar.printAll();

    return 0;
}