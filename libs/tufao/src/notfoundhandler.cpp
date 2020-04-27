#include "notfoundhandler.h"

namespace Tufao {

bool NotFoundHandler::handleRequest(HttpServerRequest *request,
                                    HttpServerResponse *response,
                                    const QStringList &arg)
{
    response->writeHead(Tufao::HttpServerResponse::NOT_FOUND);
    response->end("Not found");
    return true;
}

NotFoundHandler::NotFoundHandler( QObject *parent /*= 0*/ ) : AbstractHttpServerRequestHandler(parent)
{

}

} // namespace Tufao
