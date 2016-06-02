#include <stdio.h>
 #include <stdlib.h>
 #include <stddef.h>
 #include <stdarg.h>
 #include <string.h>
 #include <assert.h>
 #include <hiredis.h>
 
 void doTest()
 {
     int timeout = 10000;
     struct timeval tv;
     tv.tv_sec = timeout / 1000;
     tv.tv_usec = timeout * 1000;
     //�Դ��г�ʱ�ķ�ʽ����Redis��������ͬʱ��ȡ��Redis���ӵ������Ķ���
     //�ö����������������Redis�����ĺ�����
     redisContext* c = redisConnectWithTimeout("192.168.149.137",6379,tv);
     if (c->err) {
         redisFree(c);
         return;
     }
     const char* command1 = "set stest1 value1";
     redisReply* r = (redisReply*)redisCommand(c,command1);
     //��Ҫע����ǣ�������صĶ�����NULL�����ʾ�ͻ��˺ͷ�����֮��������ش��󣬱����������ӡ�
     //����ֻ�Ǿ���˵���������������������Ͳ������������ж��ˡ�
     if (NULL == r) {
         redisFree(c);
         return;
     }
     //��ͬ��Redis����ص��������Ͳ�ͬ���ڻ�ȡ֮ǰ��Ҫ���ж�����ʵ�����͡�
     //���ڸ�������ķ���ֵ��Ϣ�����Բο�Redis�Ĺٷ��ĵ������߲鿴��ϵ�в��͵�ǰ��ƪ
     //�й�Redis�����������͵Ĳ��͡�:)
     //�ַ������͵�set����ķ���ֵ��������REDIS_REPLY_STATUS��Ȼ��ֻ�е�������Ϣ��"OK"
     //ʱ���ű�ʾ������ִ�гɹ�������������Դ����ƣ��Ͳ��ٹ���׸���ˡ�
     if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str,"OK") == 0)) {
         printf("Failed to execute command[%s].\n",command1);
         freeReplyObject(r);
         redisFree(c);
         return;
     }
     //���ں����ظ�ʹ�øñ�����������Ҫ��ǰ�ͷţ������ڴ�й©��
     freeReplyObject(r);
     printf("Succeed to execute command[%s].\n",command1);
 
     const char* command2 = "strlen stest1";
     r = (redisReply*)redisCommand(c,command2);
     if (r->type != REDIS_REPLY_INTEGER) {
         printf("Failed to execute command[%s].\n",command2);
         freeReplyObject(r);
         redisFree(c);
         return;
     }
     int length = r->integer;
     freeReplyObject(r);
     printf("The length of 'stest1' is %d.\n",length);
     printf("Succeed to execute command[%s].\n",command2);
 
     const char* command3 = "get stest1";
     r = (redisReply*)redisCommand(c,command3);
     if (r->type != REDIS_REPLY_STRING) {
         printf("Failed to execute command[%s].\n",command3);
         freeReplyObject(r);
         redisFree(c);
         return;
     }
     printf("The value of 'stest1' is %s.\n",r->str);
     freeReplyObject(r);
     printf("Succeed to execute command[%s].\n",command3);
 
     const char* command4 = "get stest2";
     r = (redisReply*)redisCommand(c,command4);
     //������Ҫ��˵��һ�£�����stest2���������ڣ����Redis�᷵�ؿս��������ֻ��Ϊ����ʾ��
     if (r->type != REDIS_REPLY_NIL) {
         printf("Failed to execute command[%s].\n",command4);
         freeReplyObject(r);
         redisFree(c);
         return;
     }
     freeReplyObject(r);
     printf("Succeed to execute command[%s].\n",command4);
 
     const char* command5 = "mget stest1 stest2";
     r = (redisReply*)redisCommand(c,command5);
     //����stest2�������Redis������������ֻ�ǵڶ���ֵΪnil��
     //�����ж��ֵ���أ���Ϊ����Ӧ����������������͡�
     if (r->type != REDIS_REPLY_ARRAY) {
         printf("Failed to execute command[%s].\n",command5);
         freeReplyObject(r);
         redisFree(c);
         //r->elements��ʾ��Ԫ�ص����������������key�Ƿ���ڣ���ֵ�����������Ǽ���������
         assert(2 == r->elements);
         return;
     }
     for (int i = 0; i < r->elements; ++i) {
         redisReply* childReply = r->element[i];
         //֮ǰ�Ѿ����ܹ���get����ص�����������string��
         //���ڲ�����key�ķ���ֵ��������ΪREDIS_REPLY_NIL��
         if (childReply->type == REDIS_REPLY_STRING)
             printf("The value is %s.\n",childReply->str);
     }
     //����ÿһ����Ӧ������ʹ���ߵ����ͷţ�ֻ���ͷ����ⲿ��redisReply���ɡ�
     freeReplyObject(r);
     printf("Succeed to execute command[%s].\n",command5);
 
     printf("Begin to test pipeline.\n");
     //������ֻ�ǽ������͵�����д�뵽�����Ķ��������������У�ֱ�����ú����
     //redisGetReply����Ż��������������е�����д����Redis����������������
     //��Ч�ļ��ٿͻ����������֮���ͬ���Ⱥ�ʱ�䣬�Լ�����IO������ӳ١�
     //���ڹ��ߵľ����������ƣ����Կ��Ǹ�ϵ�в����еĹ������⡣
     if (REDIS_OK != redisAppendCommand(c,command1)
         || REDIS_OK != redisAppendCommand(c,command2)
         || REDIS_OK != redisAppendCommand(c,command3)
         || REDIS_OK != redisAppendCommand(c,command4)
         || REDIS_OK != redisAppendCommand(c,command5)) {
         redisFree(c);
         return;
     }
 
     redisReply* reply = NULL;
     //��pipeline���ؽ���Ĵ���ʽ����ǰ�����Ĵ���ʽ��ȫһֱ������Ͳ����ظ������ˡ�
     if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
         printf("Failed to execute command[%s] with Pipeline.\n",command1);
         freeReplyObject(reply);
         redisFree(c);
     }
     freeReplyObject(reply);
     printf("Succeed to execute command[%s] with Pipeline.\n",command1);
 
     if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
         printf("Failed to execute command[%s] with Pipeline.\n",command2);
         freeReplyObject(reply);
         redisFree(c);
     }
     freeReplyObject(reply);
     printf("Succeed to execute command[%s] with Pipeline.\n",command2);
 
     if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
         printf("Failed to execute command[%s] with Pipeline.\n",command3);
         freeReplyObject(reply);
         redisFree(c);
     }
     freeReplyObject(reply);
     printf("Succeed to execute command[%s] with Pipeline.\n",command3);
 
     if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
         printf("Failed to execute command[%s] with Pipeline.\n",command4);
         freeReplyObject(reply);
         redisFree(c);
     }
     freeReplyObject(reply);
     printf("Succeed to execute command[%s] with Pipeline.\n",command4);
 
     if (REDIS_OK != redisGetReply(c,(void**)&reply)) {
         printf("Failed to execute command[%s] with Pipeline.\n",command5);
         freeReplyObject(reply);
         redisFree(c);
     }
     freeReplyObject(reply);
     printf("Succeed to execute command[%s] with Pipeline.\n",command5);
     //��������ͨ��pipeline�ύ������������Ϊ���أ������ʱ��������redisGetReply��
     //���ᵼ�¸ú�������������ǰ�̣߳�ֱ�����µ�ͨ�������ύ�����������ء�
     //���Ҫ�������˳�ǰ�ͷŵ�ǰ���ӵ������Ķ���
     redisFree(c);
     return;
 }
 
 int main() 
 {
     doTest();
     return 0;
 }
 
 //���������£�
 //Succeed to execute command[set stest1 value1].
 //The length of 'stest1' is 6.
 //Succeed to execute command[strlen stest1].
 //The value of 'stest1' is value1.
 //Succeed to execute command[get stest1].
 //Succeed to execute command[get stest2].
 //The value is value1.
 //Succeed to execute command[mget stest1 stest2].
 //Begin to test pipeline.
 //Succeed to execute command[set stest1 value1] with Pipeline.
 //Succeed to execute command[strlen stest1] with Pipeline.
 //Succeed to execute command[get stest1] with Pipeline.
 //Succeed to execute command[get stest2] with Pipeline.
 //Succeed to execute command[mget stest1 stest2] with Pipeline.