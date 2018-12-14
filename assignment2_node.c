// R00155348
// program for the nodes

#include "contiki.h"
#include "net/rime/rime.h"
#include "random.h"
#include "dev/button-sensor.h"
//#include "dev/leds.h"
//#include "dev/sht11/sht11-sensor.h" // sensor library
#include <stdio.h>

// ARRAY = {HOPS, SEQUENCE}

PROCESS(node, "Node");
AUTOSTART_PROCESSES(&node);


int this_node_data(int[2] array){
	array[0] = array[0]+1; // hop count
	
	return array;
}

struct nodeInfo {
	int hops;
	int sequence_nr;
}; // the nodes dont really need a struct. its done via arrays

int node_info[2];
// code for recieving a broadcast (for the nodes 1 hop away from sink/root
broadcast_recv(struct broadcast_conn *c, const linkaddr_t *from)
{
  printf("broadcast message received from %d.%d: '%s'\n",
         from->u8[0], from->u8[1], (char *)packetbuf_dataptr());
  //struct nodeInfo thisNode;
  //thisNode = packetbuf_dataptr();
  *node_info = packetbuf_dataptr();
  node_info[0] = node_info[0] +1;
}

// code for recieving a unicast (from another leaf/node)
static void
recv_uc(struct unicast_conn *c, const linkaddr_t *from){
  printf("unicast message received from %d.%d: '%s'\n",
	 from->u8[0], from->u8[1],(char *)packetbuf_dataptr());
  //struct nodeInfo thisNode;
  //thisNode = packetbuf_dataptr();
  *node_info = packetbuf_dataptr();
  node_info[0] = node_info[0] +1;
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};
static struct unicast_conn uc;

// main thread:
// sends data recieved to the next node
// addressing ?
PROCESS_THREAD(node, ev, data){
	//struct nodeInfo thisNodePtr;
	//thisNode = &thisNode;
	PROCESS_EXITHANDLER(unicast_close(&uc));
	PROCESS_BEGIN();
	unicast_open(&uc, 146, &unicast_callbacks);
	static struct etimer et;
	while(1){
		// send this node's hop and sequence count over to the next node	
		
		linkaddr_t addr;
		etimer_set(&et, CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
	// this needs to send the node's structs
		packetbuf_copyfrom(node_info, 3);
		addr.u8[0] = 1;
		addr.u8[1] = 0;
		printf("I am %d hops away from root. Sequence %d\n",node_info[0], node_info[1]);
		if(!linkaddr_cmp(&addr, &linkaddr_node_addr)) {
			unicast_send(&uc, &addr);
		} // what does this do again?
	}
	PROCESS_END(); 
}
