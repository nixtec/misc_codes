/*
 * snc.h
 * header for snc.c
 */
#ifndef SNC_H
#define SNC_H

#include <stdio.h>		/* what to say about it ? */
#include <string.h>
#include <stdlib.h>		/* calloc(), etc */

#define INET_ADDRSTRLEN 16

/* this is simulation project, so a very flexible type system is used */

typedef enum {
  false, true
} bool;

/* binary search tree types */
typedef void *element_t;

struct node;
typedef struct node node_t;
typedef node_t *position_t;
typedef node_t *tree_t;

struct node {
  element_t e;
  tree_t left;
  tree_t right;
};



/* status of the process */
#define PROC_ACTIVE	1
#define PROC_SUSPENDED	0
/* network related types */
typedef int pid_t; /* to encapsulate process identifier */
typedef int stat_t; /* status of process [active,suspended] */
struct proc_struct {
  pid_t pid;
  /* which address the process is communicating */
  char ip_dst[INET_ADDRSTRLEN]; /* conforming to <netinet/in.h> */
  stat_t status; /* status of the process, to be triggered */
  int n_port; /* in which port this process is attached */
};
typedef struct proc_struct proc_t;

#define PORT_UP		1		/* port status */
#define PORT_DOWN	0		/* port status */
#define INVALID_PORT	-1		/* port number */
/* type for port */
struct port_struct {
  int n_port; /* port number [0 indexed] */
  stat_t status; /* status of the port */
  tree_t procs; /* list of processes on this port */
  int nprocs; /* how many processes currently attached with this port */
};
typedef struct port_struct port_t;
typedef port_t socket_t;

/* the whole network controller */
#define NPORTS 6
#define INVALID_NETWORK -1
struct network_struct {
  int nid; /* network identifier, we may have several networks [0 indexed] */
  int nports; /* number of ports for the network controller */
  port_t *ports; /* the network has 'nports' ports  */
  tree_t proc_tree; /* process tree for the network */
  /* for load balancing */
  int port_to_use; /* when new process arrives, attach it with this port */
};
typedef struct network_struct network_t;

/* types for input */

/* process initialization */
const char INIT[] = "INIT";		/*  process initiation  */
#define INIT_VAL 0
const char COMP[] = "COMP";		/*  process completion  */
#define COMP_VAL 1
const char DOWN[] = "DOWN";		/*  port is down        */
#define DOWN_VAL 2
const char REST[] = "REST";		/*  port is restored    */
#define REST_VAL 3
const char SHOW[] = "SHOW";		/*  show network status */
#define SHOW_VAL 4
const char EXIT[] = "EXIT";		/*  exit from program   */
#define EXIT_VAL 4
#define BADT_VAL 5			/*  bad type val        */
struct input_struct {
  char type[10]; /* type of input [one of the above] */
  int typeval;
  /* data for possible inputs */
  pid_t pid; /* process ID for initiation */
  char ip_dst[INET_ADDRSTRLEN]; /* ip destination address for initiation */
  int n_port; /* which port is down/restored [0 indexed ]*/
};
typedef struct input_struct input_t;


/* public functions */
void run_simulator(void);
/* first obtain a network */
/* uses static variable, not thread-safe */
network_t *init_network(void);
/* take actoin on network according to input, return the result */
int action_on_network(network_t *net, input_t *input);
/* we're done, destroy the network */
void destroy_network(network_t *net);
void show_network_status(network_t *net);
void dump_proc_tree(tree_t t, bool verbose);
void print_proc_info(proc_t *proc, bool verbose);
void update_port_to_use(network_t *net);

/* for thread-safety, use the following routines */
void network_init(network_t *net, int nid, int nports); /* initialize network */
void network_free(network_t *net);
/* now manage the obtained network */
/* high level routine that calls other functions */

int proc_init(network_t *net, proc_t *proc); /* initialize process in net */
int proc_comp(network_t *net, pid_t pid); /* process completed */
int port_down(network_t *net, int n_port); /* port is down */
int port_rest(network_t *net, int n_port); /* port is restored */
void trigger_interrupt(tree_t t, stat_t status); /* set status for all in t */
/*
 * macro equivalent of this function is available
 * int element_compare(element_t *e1, element_t *e2);
 */

/* ********* TREE FUNCTIONS ********* */
tree_t		tree_destroy(tree_t t, bool free_element); /* free element? */
position_t	tree_find(element_t e, tree_t t);
position_t	tree_find_min(tree_t t);
position_t	tree_find_max(tree_t t);
tree_t		tree_insert(element_t e, tree_t t);
tree_t		tree_delete(element_t e, tree_t t, bool free_element);
/*
 * macro equivalent of this function is available
 * element_t	tree_get_element(position_t p);
 */


/* these functions will be called several times, so using macros */
#define element_compare(e1, e2) (((proc_t*)(e1))->pid - ((proc_t *)(e2))->pid)
#define find_suitable_port(net) ((net)->port_to_use)
#define tree_get_element(p) ((p)->e)


#endif /* !SNC_H */
