#include <stdio.h>
#include "hiredis.h"

int main(int argc, char **argv)
{
    redisContext *ctx = redisConnect("127.0.0.1", 6381);
    if (ctx == NULL || ctx->err) {
        printf("Connection failed\n");
        return -1;
    }

    redisReply *reply = redisCommand(ctx, "CONFIG SET stream-node-max-bytes %d", 4096);
    if (reply == NULL) {
        printf("Command failed (%d): %s\n", ctx->err, ctx->errstr);
        return -1;
    }
    else {
        printf("Command succeeded!\n");
        printf("reply: %s\n", reply->str);
    }
    freeReplyObject(reply);
    redisFree(ctx);

    return 1;
}