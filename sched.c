#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int ID_SIZE = 50;

struct core{
  struct process* p; // pointer to the process currently running on this core
  int proc_time; // cumulative time this process has been running on the core.
  int busy;      // either 0 or 1, 0 if there is no process, 1 if there is process running on the core
};

// virtual computer struct
struct computer
{
  struct core cores[4]; //this computer has 4 cores
  long time;   // computer time in millisecond
  struct queue *q; // Linked list of processes waiting to be executed
};

// struct to store process information
struct process
{
  char * process_ID; 
  int arrival_time;   // when this process arrives (e.g., being created) 
  int service_time;  // the amount of time this process needs to run on a core to finish
  int io; // boolean io vlaue (C does not have bool value (c89/90), so use int to demonstrate boolean).
};

// one link in a queue that the scheduler uses to chain process structs, 
// this is a standard struct to create linked list which is generally leveraged to implement queue
struct node
{
  int id;
  struct process* p;
  struct node * next;
};

// Struct to manage process queue
struct queue
{
  struct node* head;
  struct node* tail;
};

int proc_num; //number of processes in the queue, waiting to be scheduled and not running
int quantum; // the amount of time in milliseconds a process can run on a core before being swapped out

//struct for computer
struct computer computer;
struct queue *q;


// Print formatted list of all nodes in queue 
void printQueue(struct queue q)
{
  struct node * cursor = q.head;

  // Traverse queue until end is reached
  while(cursor != NULL)
    {
      printf("Cursor %s:\n", cursor->p->process_ID);
      printf("arrival_time: %d\tservice_time: %d\n", cursor->p->arrival_time, cursor->p->service_time);
      printf("----------\n");
      cursor = cursor->next;
    }
}


// Create node to house process and add it to queue
void enqueue(struct queue * q, struct process * newProc)
{
  // Create new node to point to newProc
  struct node * newNode = (struct node*) malloc(sizeof(struct node));
  newNode->p = newProc;

   // Create queue if it is empty
  if(q->tail == NULL)
    {
      q->head = q->tail = newNode;
      return;
    }

  // Add new node at the end of the queue and change tail
  q->tail = q->tail->next = newNode;
  q->tail->next = NULL;
}


// Move first process in queue to back 
void moveHeadToBack(struct queue * q)
{
  struct node * cursor = malloc(sizeof(struct node));
  cursor->p = q->head->p;
  cursor->next = NULL;
  q->head = q->head->next;
  enqueue(q, cursor->p);
}

// Move process to back of queue
void rmFromQueue(struct queue * q, struct process * p)
{
  // Ensure queue is not empty and create cursor nodes
  struct node * cursor = q->head;
  struct node * prevNode = q->head;
  
  if(q->head == NULL)
    {
      printf("ERROR: EMPTY QUEUE\n");
      return;
    }  
  
  // Check whether head is desired process
  if(q->head->p == p)
    {
      cursor = cursor->next;
      free(q->head);
      q->head = cursor;
      return;
    }
  
  // Scan queue for process
  cursor = cursor->next;
  while(cursor != NULL)
    {
      if(cursor->p == p)
  	{
	  prevNode->next = cursor->next;
	  free(cursor);
  	  return;
  	}
      prevNode = prevNode->next;
      cursor = cursor->next;
    }
  
  // If process exists in queue, fucntion should not reach this point
  printf("\nERROR: PROCESS NOT FOUND IN QUEUE\n");
}


void read_file()
{
  int i,i2;
  FILE* file = fopen("input.txt", "r"); 
  char line[90];
  char name[100];
  char service_time[3];
  char arrival_time[3];
  fgets(line, sizeof(line), file);

  struct process * newProc;
  struct node * newNode;
  //  struct queue* list = (struct queue*) malloc(sizeof(struct queue));

  while (fgets(line, sizeof(line), file))
    {
      //printf("%s", line); 
      i=0;
      while(line[i]!=' '&&i<90){name[i]=line[i];i++;}
      if(i>90)break;
      name[i]=0;
      i2=++i;
      while(line[i]!=' '&&i<90){service_time[i-i2]=line[i];i++;}
      if(i>90)break;
    
      service_time[i]=0;
      i2=++i;
      while(line[i]!=' '&&i<90){arrival_time[i-i2]=line[i];i++;}
      if(i>90)break;
      arrival_time[i]=0;
    
      /* add your code here, you are to create the upcoming processes queue here.
	 essentially create a node for each process and chain them in a queue.
	 note this queue is *not* the process queue used for round robin scheduling
      */
     
      // Create new node and accompanying process
      newProc = (struct process*) malloc(sizeof(struct process));
      newProc->service_time = atoi(service_time);
      newProc->arrival_time = atoi(arrival_time);
    
      // Allocate mem for process variables
      newProc->process_ID = (char*) malloc(15 * sizeof(char));
      strcpy(newProc->process_ID, name);
    
      enqueue(computer.q, newProc);
   }

   fclose(file);
   printQueue(*computer.q);
   return;    
}


//this function call simulates one millisecond of time on the computer
void run_one_step() 
{
  int i;
  computer.time++;
  printf("\nProcessing all 4 cores, current Computer time=%lu \n",computer.time);
  for(i=0;i<4;i++)
    {
      if(computer.cores[i].busy)
	{	
	  computer.cores[i].p->service_time--; // deduct the remaining service time of the running process by one millisecond
	  computer.cores[i].proc_time++; // increment the running time for the process by one millisecond in current quantum
	  printf("Core[%d]: %s, service_time= %d,been on core for: %d \n",
		 i,computer.cores[i].p->process_ID,
		 computer.cores[i].p->service_time,
		 computer.cores[i].proc_time);
			
			
	  // you need to swap out or terminate a process if it uses up the current quantum, 
	  // or finishes all its service time. The code for this should be located in the main()
	  // function, not here.
	  // Also if your code is done right, the following warning messages should never print.
	  
	  
	  if(computer.cores[i].proc_time>quantum)
	    printf("WARNING: Process on Core[%d] should not run longer than quantum\n",i);
	  if(computer.cores[i].p->service_time<0)
	    printf("WARNING: Process on core[%d] stayed longer than its service time.\n",i);
	}
    }
}

// Generates probability-based I/O interrupts
void run_one_step_p3() 
{
  int rndm,i;
  computer.time++;
  printf("Processing all 4 cores, current Computer time=%lu \n",computer.time);
  for(i=0;i<4;i++)
    {
      if(computer.cores[i].busy)
	{	
	  if(computer.cores[i].p->io==0)
	    {
	      computer.cores[i].p->service_time--;
	      // with 10% probability, generate an io event
	      rndm=rand()%10+1;
	      if(rndm == 10)
		computer.cores[i].p->io=1;	
	    }
	  computer.cores[i].proc_time++;
	  printf("Core[%d]: process %s, service_time= %d,been on core for: %d \n",
		 i,
		 computer.cores[i].p->process_ID,
		 computer.cores[i].p->service_time,
		 computer.cores[i].proc_time);
			
			
	  // you need to swap out or terminate a process if it uses up the current quantum, has an i/o event; 
	  // or finishes all its service time. The code for this should be located in the main()
	  // function, not here.
	  // Also if your code is done right, the following warning messages should never print.
	  
	  if(computer.cores[i].p->io==1)
	    printf("WARNING: Process on core[%d] has io trigerred, please remove from core, reset io signal and place it back in queue\n",i);
	  if(computer.cores[i].proc_time>quantum)
	    printf("WARNING: Process on Core[%d] should not run longer than quantum\n",i);
	  if(computer.cores[i].p->service_time<0)
	    printf("WARNING: Process on core[%d] stayed longer than its service time.\n",i);
	}
    }
}


// NOTE: you must free struct node after taking a link off the round robin queue, and scheduling the respective 
// process to run on the core. Make sure you free the struct node to avoid memory leak.
void sched_proc(struct process * p, int core_id)
{
  if(computer.cores[core_id].busy==0)
    {
      printf("Process[%s] with service_time %d has been added to core %d\n",
	     p->process_ID,
	     p->service_time,core_id);
      computer.cores[core_id].busy=1;
      computer.cores[core_id].p=p;
      computer.cores[core_id].proc_time=0;
      proc_num --; // xx added accounting of #proc
    }
  else printf("ERROR: must call remove_proc to remove current process before adding another to the core.\n");
}

// This handles removing a process from a core, and either discarding the process if its service_time is <=0 
// or adding it to the back of the round robin queue

void remove_proc(int core_id)
{
  printf("\nProcess[%s] at core %d has been removed from core with remaining service_time=%d\n",
	 computer.cores[core_id].p->process_ID,
	 core_id,
	 computer.cores[core_id].p->service_time);
  
  // if the process has finished all its service time, terminate and clean up
  if(computer.cores[core_id].p->service_time<=0)
    {
      rmFromQueue(computer.q, computer.cores[core_id].p);
      computer.cores[core_id].busy=0;
      // free up allocated memory for process ID and struct upon termination of a process
      free(computer.cores[core_id].p->process_ID);
      free(computer.cores[core_id].p);
      computer.cores[core_id].proc_time=0;

    }
  
  // if the process needs to run for more time, put it back into the queue for future scheduling
  else
    {
      computer.cores[core_id].proc_time=0;
      
      // reinsert back to the queue
      moveHeadToBack(computer.q);
      computer.cores[core_id].busy = 0;
      computer.cores[core_id].proc_time=0;
    }
}

// a demo running 4 processes until they're finished. The scheduling is done explicitly, not using
// a scheduling algorithm. This is just to demonstrate how processes will be scheduled. In main() 
// you need to write a generic scheduling algorithm for arbitrary number of processes.
void demo()
{
  int i;
  struct process *p0,*p1,*p2,*p3;
  p0=malloc(sizeof(struct process));
  p1=malloc(sizeof(struct process));
  p2=malloc(sizeof(struct process));
  p3=malloc(sizeof(struct process));
  
  p0->process_ID=malloc(sizeof(50));//you can assume process ID will never exceed 50 characters
  p1->process_ID=malloc(sizeof(50));
  p2->process_ID=malloc(sizeof(50));
  p3->process_ID=malloc(sizeof(50));
  
  strcpy(p0->process_ID,"first");
  strcpy(p1->process_ID,"Second");
  strcpy(p2->process_ID,"Third");
  strcpy(p3->process_ID,"Fourth");
  
  //assign arrival time
  p0->arrival_time=0;
  p1->arrival_time=0;
  p2->arrival_time=0;
  p3->arrival_time=0;
  
  //assign service time
  p0->service_time=16;
  p1->service_time=17;
  p2->service_time=19;
  p3->service_time=21;
  
  // we will skip queue construction here because it's just 4 processes.
  // you must use the round robin queue for the scheduling algorithm for generic cases where many processes
  // exist and may need more than one quantum to finish
  
  
  // xx 4 processes are waiting to be scheduled. No queue is built in demo for simplicity.
  // in your generic algorithm, you should create actual queues, and proc_num should be the number of processes whose
  // arrival time has come, and are waiting in the round robin queue to be scheduled.
  proc_num=4; 
  
  
  //schedule process to each core
  sched_proc(p0,0);
  sched_proc(p1,1);
  sched_proc(p2,2);
  sched_proc(p3,3);
  
  for(i=0;i<16;i++)run_one_step();
  remove_proc(0);
  run_one_step();
  remove_proc(1);
  run_one_step();
  run_one_step();
  remove_proc(2);
  run_one_step();
  remove_proc(3);
  //	sched_proc(q->head->p,0);
  
  //NOTE: you must free struct node after scheduling the process. The demo code is not doing it here
  // for simplification, but you have to do it in your code or you will have memory leakage
  
  //head==tail since it was the only one added now to remove it we just make pointer pointing to NULL
  q->head=NULL;
  q->tail=NULL;
  run_one_step();
  remove_proc(0);
  printf("DONE\n");
}

void init()
{
  // Initialize queue
  computer.q = (struct queue*) malloc(sizeof(struct queue));
  
  quantum = 20;
}

int firstOpenCore()
{
  int i;
  for(i = 0; i < 4; i++)
    if(computer.cores[i].busy == 0)
      return i;
  return -1;
}

int main()
{
  init();
  //  printf("\t*******Starting Demo*******\n");
  //  demo();
  printf("\t*******Reading Input*******\n");
  read_file();

  printf("\t*******Scheduling Processes*******\n");

  // Variables used to run simulation
  struct node * scheduler = computer.q->head;

  int open;
  for(int i = 0; i < 500; i++)
    {
      // Fill open cores with a process
      open = firstOpenCore();
      while(open != -1)
	{
	  sched_proc(scheduler->p, open);
	  scheduler = scheduler->next;
	  open = firstOpenCore();
	}

      // Replace process if necessary (quantum or finished)
      for(int i = 0; i < 4; i++)
	if(computer.cores[i].proc_time >= quantum ||
	   computer.cores[i].p->io == 1 ||
	   computer.cores[i].p->service_time <= 0)
	  remove_proc(i);

      run_one_step();
      scheduler = computer.q->head;
    }

  
  /* sched_proc(computer.q->head->p, 0); */
  /* sched_proc(computer.q->head->next->p, 1); */
  /* for(int i = 0; i < 25; i++) */
  /*   run_one_step(); */
  
  /* your code goes here for part2. In part 2, you create one node for each process, and put them on an 
   * 'upcoming process' queue first. Then your code calls run_one_step(), for each process whose arrival time
   * has come, take the node off the 'upcoming process' queue, and place it on round robin queue. For each
   * process that's selected to run on a core, take the node off round robin queue.
   * 
   * Repeat run_one_step() until all processes finish. Please handle memory allocation/deallocation properly so there's no leak
   */
  
  
  
  
  
  
  
  /* After part 2 is done, you clean up everything, e.g., freeing up all memory allocated,
   * reset queues to empty etc.
   * Then restart for part 3: read input file for all processes, initialize queues,
   *  run processes using run_one_step_p3() so random i/o event can happen at each step on each core, 
   *  until all processes finish. Remember to clean up again at the end!
   */
  
  
  
  
  printf("\t*******Done*******\n");
  return 0;
}
