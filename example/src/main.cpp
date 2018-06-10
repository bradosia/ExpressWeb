#include <string>
#include <iostream>
#include <ResourceFileUtility.h>
#include <boost/nowide/iostream.hpp>

using namespace boost;
using namespace std;

class Callback: public ResourceFileUtility::CallbackHandler {
public:
	int fileComplete(char* filePath) {
		cout << "Sucessfully Packed: " << filePath << endl;
		return 0;
	}
	int packComplete(char* filePath) {
		cout << "Sucessfully Packed all files!" << endl;
		return 0;
	}
	int estimateFileComplete(int fileID) {
		cout << "Sucessfully Packed all files!" << endl;
		return 0;
	}
};

void CBprocess(ResourceFileUtility::ResourceFile* resourceFilePtr) {
	unsigned int sizeTotal, sizeCurrent, loadBarNumber;
	sizeTotal = resourceFilePtr->getProcessingBytesTotal();
	while (true) {
		sizeCurrent = resourceFilePtr->getProcessingBytes();
		loadBarNumber =
				(unsigned int) (sizeCurrent / (double) sizeTotal * 40.0);
		cout << std::string(loadBarNumber, '#')
				<< std::string(40 - loadBarNumber, ' ') << "  " << sizeCurrent
				<< "/" << sizeTotal << "   Progress\r";
		if (sizeCurrent >= sizeTotal)
			break;
	}
	cout << "\n";
	cout << resourceFilePtr->estimateToString();
}

int main(int argc, char** argv) {
	ExpressWeb::Environment app;
	ExpressWeb::HTTP::Request req;
	ExpressWeb::HTTP::Response res;
	if (argc < 1) {
		cout << res.headers;
		cout << res.body;
		res.end();
	} else {
		app.engine("html");
		/*
		 * app.use([path,] callback [, callback...])
		 * Mounts the specified middleware function or functions at the specified path: the middleware
		 * function is executed when the base of the requested path matches path.
		 */
		app.use(bodyParser.urlencoded( { extended: true }));
		app.use(express.cookieParser('your secret here'));
		app.use(bodyParser.json());
		app.use(express.methodOverride());
		app.use(express.session());
		app.use(app.router);
		app.use(express.static(path.join(__dirname, 'public')));

				app.route.get(filesystem::path("/"),
						[](ExpressWeb::Request& req, ExpressWeb::Response& res) {
							// nothing
						});
				cout << res.headers;
				cout << res.body;
				res.end();
			}
			// Keep the console window open in debug mode.
			string temp;
			cout << "Press any key to exit." << endl;
			getline(cin, temp);
			return 0;
		}
