#include "mongoose.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

const char admin[] = "admin";
const char adminPassword[] = "admin123";

//数据结构
typedef struct Student {
    char id[20];
    char name[50];
    char password[50];
    char phone[20];
    char class_name[50];
    struct Student *next;
} Student;

typedef struct EnrolledNode {
    char student_id[20];
    struct EnrolledNode *next;
} EnrolledNode;

typedef struct Activity {
    int id;
    char name[100];
    char category[50];
    char location[100];
    int max_capacity;
    char deadline[25];
    char status[20];
    int current_count;
    EnrolledNode *enrolled_head;
    struct Activity *next;
} Activity;

Student *student_head = NULL;
Activity *activity_head = NULL;
int global_id_counter = 1000;

//获取当前时间字符串
void get_current_time_str(char *buffer) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    sprintf(buffer, "%04d%02d%02d",
            tm_info->tm_year + 1900,
            tm_info->tm_mon + 1,
            tm_info->tm_mday);
}

//检查活动是否结束
void check_expiry(Activity *act) {
    char now[25];
    get_current_time_str(now);
    if (strcmp(now, act->deadline) > 0) {
        strcpy(act->status, "已结束");
    } else {
        // 如果未过期且当前状态是已结束，恢复为报名中(可选)
        if (strcmp(act->status, "已结束") == 0)
            strcpy(act->status, "报名中");
    }
}

//校验学号 4位年份+3位编号  admin保留
int validate_id(const char *id) {
    if (strcmp(id, admin) == 0 || strcmp(id, "ADMIN") == 0) return -1;
    if (strlen(id) != 7) return 0;
    if (id[0] != '2') return 0;
    for (int i = 0; i < 7; i++) if (!isdigit(id[i])) return 0;
    return 1;
}

int get_json_int(const char *json, const char *key) {
    char search[64];
    const char *p;
    sprintf(search, "\"%s\":", key);
    p = strstr(json, search);
    if (!p) return -1;
    return atoi(p + strlen(search));
}

void get_json_str(const char *json, const char *key, char *dest) {
    char search[64];
    const char *p;
    int i = 0;
    sprintf(search, "\"%s\":\"", key);
    p = strstr(json, search);
    if (!p) {
        dest[0] = '\0';
        return;
    }
    p += strlen(search);
    while (*p != '\"' && *p != '\0') dest[i++] = *p++;
    dest[i] = '\0';
}

//数据存储
void save_students() {
    FILE *fp = fopen("Data/student.txt", "w");
    if (!fp) return;
    Student *curr = student_head;
    while (curr) {
        fprintf(fp, "%s %s %s %s %s\n", curr->id, curr->name, curr->password, curr->phone, curr->class_name);
        curr = curr->next;
    }
    fclose(fp);
}

void load_students() {
    FILE *fp = fopen("Data/student.txt", "r");
    if (!fp) return;
    char id[20], n[50], p[50], ph[20], c[50];
    while (fscanf(fp, "%s %s %s %s %s", id, n, p, ph, c) != EOF) {
        Student *node = (Student *) malloc(sizeof(Student));
        strcpy(node->id, id);
        strcpy(node->name, n);
        strcpy(node->password, p);
        strcpy(node->phone, ph);
        strcpy(node->class_name, c);
        node->next = student_head;
        student_head = node;
    }
    fclose(fp);
}

void save_activities() {
    FILE *fp = fopen("Data/activity.txt", "w");
    if (!fp) return;
    Activity *curr = activity_head;
    while (curr) {
        check_expiry(curr); // 保存前更新状态
        fprintf(fp, "%d %s %s %s %d %s %s ", curr->id, curr->name, curr->category, curr->location, curr->max_capacity,
                curr->status, curr->deadline);
        if (!curr->enrolled_head) fprintf(fp, "None");
        else {
            EnrolledNode *e = curr->enrolled_head;
            while (e) {
                fprintf(fp, "%s", e->student_id);
                if (e->next) fprintf(fp, "|");
                e = e->next;
            }
        }
        fprintf(fp, "\n");
        curr = curr->next;
    }
    fclose(fp);
}

void load_activities() {
    FILE *fp = fopen("Data/activity.txt", "r");
    if (!fp) return;
    int id, max;
    char name[100], cat[50], loc[100], stat[20], ddl[25], list[4096];
    while (fscanf(fp, "%d %s %s %s %d %s %s %s", &id, name, cat, loc, &max, stat, ddl, list) != EOF) {
        Activity *node = (Activity *) malloc(sizeof(Activity));
        node->id = id;
        if (id > global_id_counter) global_id_counter = id;
        strcpy(node->name, name);
        strcpy(node->category, cat);
        strcpy(node->location, loc);
        node->max_capacity = max;
        strcpy(node->status, stat);
        strcpy(node->deadline, ddl);
        node->enrolled_head = NULL;
        node->current_count = 0;
        if (strcmp(list, "None") != 0) {
            char *token = strtok(list, "|");
            while (token) {
                EnrolledNode *en = (EnrolledNode *) malloc(sizeof(EnrolledNode));
                strcpy(en->student_id, token);
                en->next = node->enrolled_head;
                node->enrolled_head = en;
                node->current_count++;
                token = strtok(NULL, "|");
            }
        }
        check_expiry(node); // 加载时立即检查
        node->next = activity_head;
        activity_head = node;
    }
    fclose(fp);
}

//run
int register_student(const char *id, const char *name, const char *pwd, const char *ph, const char *cls) {
    Student *c = student_head;
    while (c) {
        if (strcmp(c->id, id) == 0) return 0;
        c = c->next;
    }
    Student *n = (Student *) malloc(sizeof(Student));
    strcpy(n->id, id);
    strcpy(n->name, name);
    strcpy(n->password, pwd);
    strcpy(n->phone, ph);
    strcpy(n->class_name, cls);
    n->next = student_head;
    student_head = n;
    save_students();
    return 1;
}

int enroll_student(int act_id, const char *stu_id) {
    Activity *curr = activity_head;
    while (curr) {
        if (curr->id == act_id) {
            check_expiry(curr);
            if (strcmp(curr->status, "已结束") == 0) return -2;
            if (curr->current_count >= curr->max_capacity) return -1;
            EnrolledNode *chk = curr->enrolled_head;
            while (chk) {
                if (strcmp(chk->student_id, stu_id) == 0) return 0;
                chk = chk->next;
            }
            EnrolledNode *node = (EnrolledNode *) malloc(sizeof(EnrolledNode));
            strcpy(node->student_id, stu_id);
            node->next = curr->enrolled_head;
            curr->enrolled_head = node;
            curr->current_count++;
            save_activities();
            return 1;
        }
        curr = curr->next;
    }
    return -404;
}

void delete_activity(const int id) {
    Activity *curr = activity_head, *prev = NULL;
    while (curr) {
        if (curr->id == id) {
            FILE *f = fopen("Data/trash.txt", "a");
            if (f) {
                fprintf(f, "%d %s %s %s %d %s %s ", curr->id, curr->name, curr->category, curr->location, curr->max_capacity,curr->status, curr->deadline);
            }
            if (prev) prev->next = curr->next;
            else activity_head = curr->next;
            if (!curr->enrolled_head) fprintf(f, "None");
            else while (curr->enrolled_head) {
                EnrolledNode *t = curr->enrolled_head;
                curr->enrolled_head = t->next;
                fprintf(f, "%s", t->student_id);
                if (t->next) fprintf(f, "|");
                free(t);
            }
            fprintf(f, "\n");
            free(curr);
            save_activities();
            fclose(f);
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

//处理前端请求
static void fn(struct mg_connection *c, int ev, void *ev_data, void *fn_data) {
    if (ev == MG_EV_HTTP_MSG) {
        struct mg_http_message *hm = (struct mg_http_message *) ev_data;
        char body[2048] = {0};
        if (hm->body.len > 0 && hm->body.len < 2047)
            memcpy(body, hm->body.buf, hm->body.len);

        if (mg_match(hm->uri, mg_str("/api/login"), NULL)) {
            char u[50] = {0}, p[50] = {0};
            get_json_str(body, "username", u);
            get_json_str(body, "password", p);
            if (strcmp(u, admin) == 0 && strcmp(p, adminPassword) == 0) mg_http_reply(
                c, 200, "", "{\"status\":\"success\",\"role\":\"admin\"}");
            else {
                Student *s = student_head;
                int f = 0;
                while (s) {
                    if (strcmp(s->id, u) == 0 && strcmp(s->password, p) == 0) {
                        f = 1;
                        break;
                    }
                    s = s->next;
                }
                if (f) mg_http_reply(c, 200, "", "{\"status\":\"success\",\"role\":\"student\"}");
                else mg_http_reply(c, 200, "", "{\"status\":\"fail\",\"msg\":\"账号密码错误\"}");
            }
        } else if (mg_match(hm->uri, mg_str("/api/register"), NULL)) {
            char id[20] = {0}, n[50] = {0}, p[50] = {0}, ph[20] = {0}, cl[50] = {0};
            get_json_str(body, "id", id);
            get_json_str(body, "name", n);
            get_json_str(body, "password", p);
            get_json_str(body, "phone", ph);
            get_json_str(body, "class", cl);
            int val = validate_id(id);
            if (val == -1) mg_http_reply(c, 200, "", "{\"status\":\"error\",\"msg\":\"admin为保留账号\"}");
            else if (val == 0) mg_http_reply(c, 200, "", "{\"status\":\"error\",\"msg\":\"学号须为4位年份+3位编号\"}");
            else if (strlen(n) == 0) mg_http_reply(c, 200, "", "{\"status\":\"error\",\"msg\":\"姓名不能为空\"}");
            else {
                if (register_student(id, n, p, ph, cl)) mg_http_reply(c, 200, "", "{\"status\":\"success\"}");
                else mg_http_reply(c, 200, "", "{\"status\":\"error\",\"msg\":\"学号已存在\"}");
            }
        } else if (mg_match(hm->uri, mg_str("/api/activities"), NULL)) {
            char *json = (char *) malloc(1024 * 64);
            strcpy(json, "[");
            Activity *curr = activity_head;
            char item[1024];
            while (curr) {
                check_expiry(curr);
                sprintf(
                    item,
                    "{\"id\":%d, \"name\":\"%s\", \"category\":\"%s\", \"location\":\"%s\", \"deadline\":\"%s\", \"max\":%d, \"count\":%d, \"status\":\"%s\"}",
                    curr->id, curr->name, curr->category, curr->location, curr->deadline, curr->max_capacity,
                    curr->current_count, curr->status);
                strcat(json, item);
                if (curr->next)
                    strcat(json, ",");
                curr = curr->next;
            }
            strcat(json, "]");
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", json);
            free(json);
        } else if (mg_match(hm->uri, mg_str("/api/add"), NULL)) {
            char name[100] = {0}, cat[50] = {0}, loc[100] = {0}, ddl[25] = {0};
            int max = get_json_int(body, "max");
            get_json_str(body, "name", name);
            get_json_str(body, "category", cat);
            get_json_str(body, "location", loc);
            get_json_str(body, "deadline", ddl);
            if (ddl[4]=='1'&&ddl[5]>'2'&&ddl[5]<='9') {
                mg_http_reply(c, 200, "", "{\"status\":\"failed\",\"msg\":\"非法的时间格式\"}");
            }else {
                Activity *node = (Activity *) malloc(sizeof(Activity));
                node->id = ++global_id_counter;
                strcpy(node->name, name);
                strcpy(node->category, cat);
                strcpy(node->location, loc);
                strcpy(node->deadline, ddl);
                node->max_capacity = max;
                strcpy(node->status, "报名中");
                node->current_count = 0;
                node->enrolled_head = NULL;
                node->next = activity_head;
                activity_head = node;
                save_activities();
                mg_http_reply(c, 200, "", "{\"status\":\"added\",\"msg\":\"Time\"}");
            }

        } else if (mg_match(hm->uri, mg_str("/api/enroll"), NULL)) {
            int aid = get_json_int(body, "id");
            char sid[20] = {0};
            get_json_str(body, "student_id", sid);
            int r = enroll_student(aid, sid);
            if (r == 1) mg_http_reply(c, 200, "", "{\"status\":\"success\"}");
            else if (r == 0) mg_http_reply(c, 200, "", "{\"status\":\"repeat\"}");
            else if (r == -1) mg_http_reply(c, 200, "", "{\"status\":\"full\"}");
            else if (r == -2) mg_http_reply(c, 200, "", "{\"status\":\"closed\"}");
            else mg_http_reply(c, 200, "", "{\"status\":\"error\"}");
        } else if (mg_match(hm->uri, mg_str("/api/delete"), NULL)) {
            delete_activity(get_json_int(body, "id"));
            mg_http_reply(c, 200, "", "{\"status\":\"deleted\"}");
        } else if (mg_match(hm->uri, mg_str("/api/details"), NULL)) {
            int id = get_json_int(body, "id");
            char *buf = (char *) malloc(1024 * 50);
            strcpy(buf, "[");
            Activity *curr = activity_head;
            while (curr) {
                if (curr->id == id) {
                    EnrolledNode *e = curr->enrolled_head;
                    int first = 1;
                    while (e) {
                        Student *s = student_head;
                        while (s) {
                            if (strcmp(s->id, e->student_id) == 0) {
                                if (!first)
                                    strcat(buf, ",");
                                char item[512];
                                // 注意：这里的 Key 分别是 id, name, classname, phone
                                // 前端 JS 必须用 s.classname 来获取班级
                                sprintf(
                                    item, "{\"id\":\"%s\", \"name\":\"%s\", \"classname\":\"%s\", \"phone\":\"%s\"}",
                                    s->id, s->name, s->class_name, s->phone);
                                strcat(buf, item);
                                first = 0;
                                break;
                            }
                            s = s->next;
                        }
                        e = e->next;
                    }
                    break;
                }
                curr = curr->next;
            }
            strcat(buf, "]");
            mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s", buf);
            free(buf);
        } else if (mg_match(hm->uri, mg_str("/api/stats"), NULL)) {
            int a = 0, b = 0, cs = 0, d = 0;
            Activity *cu = activity_head;
            while (cu) {
                if (strcmp(cu->category, "学术") == 0)a++;
                else if (strcmp(cu->category, "文艺") == 0)b++;
                else if (strcmp(cu->category, "体育") == 0)cs++;
                else d++;
                cu = cu->next;
            }
            mg_http_reply(c, 200, "Content-Type: application/json\r\n",
                          "{\"academic\":%d,\"arts\":%d,\"sports\":%d,\"other\":%d}", a, b, cs, d);
        }else if (mg_match(hm->uri,mg_str("/api/change"),NULL)){
            int aid=get_json_int(body,"aid");
            char name[100] = {0}, cat[50] = {0}, loc[100] = {0}, ddl[25] = {0};
            int max = get_json_int(body, "max");
            get_json_str(body, "name", name);
            get_json_str(body, "category", cat);
            get_json_str(body, "location", loc);
            get_json_str(body, "deadline", ddl);
            Activity *curr=activity_head;
            int flag = 0;
            while (curr) {
                if (curr->id == aid) {
                    if (ddl[4]=='1'&&ddl[5]>'2'&&ddl[5]<='9') {
                        flag=2;
                        break;
                    }
                    strcpy(curr->name, name);
                    strcpy(curr->category, cat);
                    strcpy(curr->location, loc);
                    strcpy(curr->deadline, ddl);
                    curr->max_capacity = max;
                    strcpy(curr->status, "报名中");
                    flag = 1;
                    save_activities();//立即保存
                    break;
                }
            }
            if (flag == 1) {
                mg_http_reply(c, 200, "", "{\"status\":\"success\",\"msg\":\"None\"}");
            }else if (flag == 2){
                mg_http_reply(c, 200, "", "{\"status\":\"failed\",\"msg\":\"非法的时间格式\"}");
            }else {
                mg_http_reply(c, 200, "", "{\"status\":\"error\",\"msg\":\"不存在的活动\"}");
            }
        } else {
            struct mg_http_serve_opts o = {.root_dir = "./frontend"};
            mg_http_serve_dir(c, hm, &o);
        }
    }
}

int main() {
    struct mg_mgr mgr;
    load_students();
    load_activities();
    mg_mgr_init(&mgr);//初始化消息队列
    mg_http_listen(&mgr, "http://localhost:8000", (mg_event_handler_t) fn, NULL);
    printf("System running at http://localhost:8000\n");
    for (;;) mg_mgr_poll(&mgr, 1000);
    mg_mgr_free(&mgr);
    return 0;
}
