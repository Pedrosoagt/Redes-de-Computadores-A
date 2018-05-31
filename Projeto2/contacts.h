#ifndef CONTACTS_H
#define CONTACTS_H

#include <netinet/in.h>
#include <stdbool.h>

//estrutura de usuário
struct contact_t {
  char num[16]; // numero de contato
  struct sockaddr_in *local; //Ip + Porta
};

//Lista de usuário
struct contactCollection_t {
  struct contact_t info;
  struct contactCollection_t *next;
};

typedef struct contact_t Contact;
typedef struct contactCollection_t ContactCollection;

ContactCollection* createCollection();
bool createContact(ContactCollection **, Contact);
ContactCollection* findContact(ContactCollection *, char *);
bool updateContact(ContactCollection **, Contact);
void printContacts(ContactCollection *);

#endif
