#include "XmlParser.h"
#include "XmlNode.h"

int main()
{	// command line processor 			
	XmlParser parser;
    XmlNodeMap JunkMap;

    const XmlNode *config = parser.ReturnXMLDocument("FileToStress.xml");

    for (int i = 0; i < 1000; i++) {

    	try
    	{	

            JunkMap.DeepCopy(config->getChildren());
            for(int j = 1; j < JunkMap.size(); j++){
                  if(string(JunkMap.getNode(j)->getName().c_str()) != string(config->getChildren().getNode(j)->getName().c_str()))
                      cout<<"Node Name Failure "<<JunkMap.getNode(j)->getName().c_str()<<" "<<config->getChildren().getNode(j)->getName().c_str()<<'\n';
                  //if(string(JunkMap.getNode(j)->getAttributes()) != string(config->getChildren().getNode(j)->getAttributes()))
                  //    cout<<"Node Attributes Failure "<<JunkMap.getNode(j)->getName().c_str()<<" "<<config->getChildren().getNode(j)->getName().c_str()<<'\n';
            }
            
            
    	}
    	catch(XmlException &XmlErr)
    	{
    		printf("Failure 1 - %s\n", XmlErr.what());
            cout<<"Run Number "<<i<<'\n';
    	}
    	catch(BepException &BepErr)
    	{
    		printf("Failure 2 - %s\n", BepErr.what());
    	}
    	catch(...)
    	{
    		printf("Failure 3\n");
    	}

        //JunkMap = NULL;
        //config = NULL;
        if(i%25 == 0)
            cout<<i<<'\n';


    }

    printf("All Done\n");

    return 1;
}
