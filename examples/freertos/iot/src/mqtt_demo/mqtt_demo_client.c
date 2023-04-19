// Copyright 2020-2022 XMOS LIMITED.
// This Software is subject to the terms of the XMOS Public Licence: Version 1.

#define DEBUG_UNIT MQTT_DEMO_CLIENT
#include <string.h>

/* FreeRTOS headers */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* FreeRTOS Plus headers */
#include "FreeRTOS_IP.h"
#include "FreeRTOS_Sockets.h"

/* Library headers */
#include "rtos_gpio.h"
#include "tls_support.h"
#include "MQTTClient.h"

/* App headers */
#include "app_conf.h"
#include "mqtt_demo_client.h"
#include "jsmn.h"
#include "sntpd.h"

#ifdef MBEDTLS_DEBUG_C
#include "mbedtls/debug.h"
#endif

#define MQTT_DEMO_CONNECT_STACK_SIZE 		2400
#define MQTT_DEMO_HANDLER_STACK_SIZE 		1600
#define MQTT_RECONNECT_DELAY_MS				1000
#define MQTT_RECONNECT_DELAY_STEP_MS		1000
#define MQTT_RECONNECT_DELAY_MAX_MS		   15000

const char* HOSTNAME = appconfMQTT_HOSTNAME;
const char* client_str = appconfMQTT_CLIENT_ID;
const char* topic = appconfMQTT_DEMO_TOPIC;
const char* mic_topic = appconfMQTT_MIC_TOPIC;

typedef struct net_conn_args
{
	Socket_t socket;
	mbedtls_ssl_context* ssl_ctx;
	TaskHandle_t connection_task;
} net_conn_args_t;

static rtos_gpio_t* gpio = NULL;
static rtos_gpio_port_id_t led_port = 0;
static uint32_t val;

#define MAX_TOKENS 10

//--------------------------------//
//Added params

#include <stdio.h>
#include <stdint.h>
// #include "mic_array.h"; //No library :(
#include <math.h>
// #include <mosquitto.h> //or whatever has the mqtt_publish command
static int SAMPLE_RATE = 16000; //I think this is right
static int CHANNELS = 1; //Stereo baby!
static float PACKET_TIME = 0.02; //Packet time in seconds


// typedef struct {
//     // WAV file header fields
//     char     riff[4];         // "RIFF"
//     uint32_t file_size;       // total file size - 8 bytes
//     char     wave[4];         // "WAVE"
//     char     fmt[4];          // "fmt "
//     uint32_t fmt_size;        // size of the fmt chunk (16 bytes)
//     uint16_t audio_format;    // audio format (1 for PCM)
//     uint16_t num_channels;    // number of channels
//     uint32_t sample_rate;     // sample rate
//     uint32_t byte_rate;       // byte rate (num_channels * sample_rate * bits_per_sample / 8)
//     uint16_t block_align;     // block align (num_channels * bits_per_sample / 8)
//     uint16_t bits_per_sample; // bits per sample
//     char     data[4];         // "data"
//     uint32_t data_size;       // size of the audio data
// } WAVHeader;

void publish_test(MQTTClient* client){

	MQTTMessage send;
	send.qos 			= 2;//ensure packets arrive in order!
	send.retained 		= 0;
	send.dup			= 'a';
	send.id				= '0';
	send.payload		= "Starting audio stream:";
	send.payloadlen		= 22;

	MQTTPublish(client, mic_topic, &send);
	debug_printf("Sent test publish\n");

	int SAMPLE_SIZE = SAMPLE_RATE * CHANNELS * PACKET_TIME;

	debug_printf("Initilized samples with sample size %d \n", SAMPLE_SIZE);

	uint16_t audio_packet[CHANNELS]; //Start the audio array
	audio_packet[0] = '\0'; //Initilize with null character for some reason?
	debug_printf("Initilized packet\n");
	
	// WAVHeader header = {
	// 	.riff = {'R', 'I', 'F', 'F'},
	// 	.wave = {'W', 'A', 'V', 'E'},
	// 	.fmt = {'f', 'm', 't', ' '},
	// 	.fmt_size = 8,
	// 	.audio_format = 1,
	// 	.num_channels = 2,
	// 	.sample_rate = SAMPLE_RATE,
	// 	.bits_per_sample = SAMPLE_SIZE,
	// 	.data = {'d', 'a', 't', 'a'},
 	// };

	float w = 2 * M_PI * 700; //700hz test tone
	int k=1;
	debug_printf("Starting stream:\n");
	for (;;) {
		
		if ( k >= 20 ){ k = 1 ; }

		for (int i=0; i<SAMPLE_SIZE; i++){
			debug_printf("filling audio: %d \n", i);
			for (int j=0; j <= CHANNELS; j++){
				// audio_packet[i*2+j] = (uint16_t)(32767 * sinf(w*(i+(SAMPLE_SIZE*k))));
				audio_packet[i*2+j] = (uint16_t)(i);

			}
		}

		k++;
		
		debug_printf("Setting Packet\n");
		send.payload	= audio_packet;
		debug_printf("Setting Size\n");
		send.payloadlen	= SAMPLE_SIZE;
		debug_printf("Sending\n");
		MQTTPublish(client, mic_topic, &send); //Publish audio data to MQTT every time loop runs!
		// delay(SAMPLE_TIME); //Should delay, don't know what package to import for it
		debug_printf("Audio sent!\n");
	}
	debug_printf("HOW?\n");
}

void messageArrived(MessageData* data) //IMPORTANT
{
	char* payload = data->message->payload;
	int payload_len = data->message->payloadlen;
	jsmn_parser p; //EVERYTHING PAST HERE IS FOR THE JSON STUFF!
	jsmntok_t tkn[MAX_TOKENS];
	int r;
	// publish_mic_audio(); //just shoving it here to test

	jsmn_init(&p);
	r = jsmn_parse(&p, payload, payload_len, tkn, MAX_TOKENS );
	debug_printf("r:%d\n",r);

	

	for( int i=0; i<r; i++ )
	{
		debug_printf("json[%d]:%.*s\n", i, tkn[i].end - tkn[i].start, payload + tkn[i].start);
	}

	if( strstr( payload, "0" ) != NULL )
	{
		if( strstr( payload, "on" ) != NULL )
		{
			val |= ( 1 << 0 );
		}
		else if( strstr( payload, "off" ) != NULL )
		{
			val &= ~( 1 << 0 );
		}
	}
	else if( strstr( payload, "1" ) != NULL )
	{
		if( strstr( payload, "on" ) != NULL )
		{
			val |= ( 1 << 1 );
		}
		else if( strstr( payload, "off" ) != NULL )
		{
			val &= ~( 1 << 1 );
		}
	}
	else if( strstr( payload, "2" ) != NULL )
	{
		if( strstr( payload, "on" ) != NULL )
		{
			val |= ( 1 << 2 );
		}
		else if( strstr( payload, "off" ) != NULL )
		{
			val &= ~( 1 << 2 );
		}
	}
	else if( strstr( payload, "3" ) != NULL )
	{
		if( strstr( payload, "on" ) != NULL )
		{
			val |= ( 1 << 3 );
		}
		else if( strstr( payload, "off" ) != NULL )
		{
			val &= ~( 1 << 3 );
		}
	}

	// publish_test(data);

    rtos_gpio_port_out(gpio, led_port, val);

	debug_printf("Message arrived on topic %.*s: %.*s\n", data->topicName->lenstring.len, data->topicName->lenstring.data,
		data->message->payloadlen, data->message->payload);
}


static void mqtt_handler( void* arg )
{
	net_conn_args_t* args = ( net_conn_args_t* ) arg;
	MQTTClient client;
	Network network;
	unsigned char sendbuf[1024], readbuf[1024];
	int retval = 0;

	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;

	val = 0x00;
    rtos_gpio_port_out(gpio, led_port, val);

	TaskHandle_t caller = args->connection_task;

	NetworkInit( &network );
	MQTTClientInit( &client, &network, 30000, sendbuf, sizeof( sendbuf ), readbuf, sizeof( readbuf ) );

	network.my_socket = args->socket;
	network.ssl_ctx = args->ssl_ctx;

    MQTTStartTask( &client );

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = ( char * )client_str;
	
	for( ;; )
	{
		if( ( retval = MQTTConnect( &client, &connectData ) ) != 0 )
		{
			debug_printf("Return code from MQTT connect is %d\n", retval);
			break;
		}
		else
		{
			debug_printf("MQTT Connected\n");
			
			debug_printf("Started Publisher on mic_topic\n");
			publish_test(&client);
			

			if( ( retval = MQTTSubscribe( &client, topic, QOS1, messageArrived ) ) != 0 )
			{
				debug_printf("Return code from MQTT subscribe is %d\n", retval);
			}
			else
			{
				debug_printf("MQTT Subscribed\n");

				while( 1 )
				{
					vTaskDelay( pdMS_TO_TICKS( 1000 ) );
					if( MQTTIsConnected( &client ) == 0 )
					{
						break;
					}
				}
				break;
			}
			
		}
	}
	debug_printf("MQTT Cleanup\n");
	vTaskDelete( client.thread.task );
	vTaskResume( caller );
	vTaskDelete( NULL );
}

static void mqtt_demo_connect( void* arg )
{
	( void ) arg;
	TaskHandle_t handler_task;
	int tmpval = 0;
	struct freertos_sockaddr sAddr;
	int recv_timeout = pdMS_TO_TICKS( 5000 );
	int send_timeout = pdMS_TO_TICKS( 5000 );

	uint32_t ip;

	while( FreeRTOS_IsNetworkUp() != pdTRUE )
	{
		vTaskDelay( pdMS_TO_TICKS(100) );
	}

	while( 1 )
	{
		debug_printf("Try to get host ip\n");
		ip = FreeRTOS_gethostbyname( HOSTNAME );

		if( ip > 0 )
		{
			break;
		}
		vTaskDelay( pdMS_TO_TICKS(100) );
	}
	debug_printf("Got ip\n");

	sAddr.sin_addr = ip;
	
	/* TODO make these args */
	sAddr.sin_port = FreeRTOS_htons( appconfMQTT_PORT );
	Socket_t socket;
	/* These can be shared by multiple connections */
	mbedtls_x509_crt* cert = pvPortMalloc( sizeof( mbedtls_x509_crt ) );
	mbedtls_pk_context* prvkey = pvPortMalloc( sizeof( mbedtls_pk_context ) );
	mbedtls_x509_crt* ca = pvPortMalloc( sizeof( mbedtls_x509_crt ) );
	// while( ( tls_platform_ready() == 0 ) || ( is_time_synced() == 0 ) )
	// {
	// 	vTaskDelay( pdMS_TO_TICKS( 100 ) );
	// } //removed to fake tls cert because we can't reach smtp time servers :/
	debug_printf("Faking time\n");
	rtos_time_t faked_time;
	faked_time.seconds = 168185455; //THIS WAS WORKING WHY IS IT NOT NOW???
	faked_time.microseconds = 0;
	rtos_time_set(faked_time);
	mbedtls_x509_crt_init( cert );
	mbedtls_pk_init( prvkey );
	mbedtls_x509_crt_init( ca );

	get_device_cert( cert );
	get_device_prvkey( prvkey );
	get_ca_cert( ca );
	/* These can be shared by multiple connections, if same credentials are used */
	mbedtls_ssl_config* ssl_conf  = pvPortMalloc( sizeof( mbedtls_ssl_config ) );
	mbedtls_ssl_config_init( ssl_conf );
	if( mbedtls_ssl_config_defaults( ssl_conf,
									 MBEDTLS_SSL_IS_CLIENT,
									 MBEDTLS_SSL_TRANSPORT_STREAM,
									 MBEDTLS_SSL_PRESET_DEFAULT )
			!= 0 )
	{
		configASSERT(0); /* Failed to initialize ssl conf with defaults */
	}
	if( mbedtls_ssl_conf_own_cert( ssl_conf, cert, prvkey ) != 0 )
	{
		configASSERT(0); /* Failed to initialize ssl conf with device cert and key */
	}
	mbedtls_ctr_drbg_context *drbg_ptr = get_shared_drbg_ctx();
	mbedtls_ssl_conf_rng( ssl_conf, mbedtls_ctr_drbg_random, drbg_ptr );

	mbedtls_ssl_conf_authmode( ssl_conf, MBEDTLS_SSL_VERIFY_REQUIRED );
	mbedtls_ssl_conf_ca_chain( ssl_conf, ca, NULL );

#ifdef MBEDTLS_DEBUG_C
	/* Add debug function to the conf */
	mbedtls_ssl_conf_dbg( ssl_conf, default_mbedtls_debug, NULL );
	mbedtls_debug_set_threshold(4);
#endif
	debug_printf("Trying the handshake...\n");
	/* SSL context is per connection */
	mbedtls_ssl_context* ssl_ctx = pvPortMalloc( sizeof( mbedtls_ssl_context ) );

	tls_ctx_t* tls_ctx = pvPortMalloc( sizeof( tls_ctx_t ) );

	int mqtt_timeout = MQTT_RECONNECT_DELAY_MS;
	for( ;; )
	{
		tls_ctx_init( tls_ctx );
		mbedtls_ssl_init( ssl_ctx );

		if( mbedtls_ssl_setup( ssl_ctx, ssl_conf ) != 0 )
		{
			configASSERT(0); /* Failed to initialize ssl context */
		}

		// if( ( mbedtls_ssl_set_hostname( ssl_ctx, HOSTNAME ) ) != 0 )
		// {
		// 	configASSERT( 0 ); /* set hostname failed */
		// }

		mbedtls_ssl_set_bio( ssl_ctx, tls_ctx, tls_send, tls_recv, NULL );

		while( FreeRTOS_IsNetworkUp() == pdFALSE )
		{
			vTaskDelay( pdMS_TO_TICKS( 100 ) );
		}

		socket = FreeRTOS_socket( FREERTOS_AF_INET,
								  FREERTOS_SOCK_STREAM,
								  FREERTOS_IPPROTO_TCP );

		/* Check the socket was created. */
		configASSERT( socket != FREERTOS_INVALID_SOCKET );

		/* Set time outs */
		FreeRTOS_setsockopt( socket,
							 0,
							 FREERTOS_SO_RCVTIMEO,
							 &recv_timeout,
							 sizeof( TickType_t ) );

		FreeRTOS_setsockopt( socket,
							 0,
							 FREERTOS_SO_SNDTIMEO,
							 &send_timeout,
							 sizeof( TickType_t ) );
		debug_printf("Shaking...\n");
		if(	FreeRTOS_connect( socket, &sAddr, sizeof( sAddr ) ) == 0 )
		{
			tls_ctx->socket = socket;
			/* attempt to handshake */
			while( ( tmpval = mbedtls_ssl_handshake( ssl_ctx ) ) != 0 )
			{
				if( tmpval != MBEDTLS_ERR_SSL_WANT_READ && tmpval != MBEDTLS_ERR_SSL_WANT_WRITE )
				{
					debug_printf( "mbedtls_ssl_handshake returned -0x%x\n\n", (unsigned int) -tmpval );
					break;
				}
			}

			if( tmpval == 0 )
			{
				/* Reset timeout on successful handshake */
				mqtt_timeout = MQTT_RECONNECT_DELAY_MS;

				/* verify peer cert */
				if( ( tmpval = mbedtls_ssl_get_verify_result( ssl_ctx ) ) != 0 )
				{
					debug_printf("failed to verify peer cert\n");
				}
				else
				{
					/* Call MQTT task */
					net_conn_args_t* handler_args = pvPortMalloc( sizeof( net_conn_args_t ) );

					handler_args->socket = socket;
					handler_args->ssl_ctx = ssl_ctx;
					handler_args->connection_task = xTaskGetCurrentTaskHandle();

					xTaskCreate( mqtt_handler, "mqtt", MQTT_DEMO_HANDLER_STACK_SIZE, ( void * ) handler_args, uxTaskPriorityGet( NULL ) + 1, &handler_task );
//IMPORTANT!!!
					vTaskSuspend( NULL );
					vPortFree( handler_args );
				}
				mbedtls_ssl_close_notify( ssl_ctx );
			}
			else
			{
				mqtt_timeout += MQTT_RECONNECT_DELAY_STEP_MS;
			}

			FreeRTOS_shutdown( socket, FREERTOS_SHUT_RDWR );

			int cnt = 0;

			char* dummy_buf = pvPortMalloc( sizeof( char ) * 1024 );
			while( FreeRTOS_recv( socket, dummy_buf, 1024, FREERTOS_MSG_DONTWAIT ) >= 0 )
			{
				if( ++cnt > 1000 )
				{
					/* Timeout */
					break;
				}
				vTaskDelay( pdMS_TO_TICKS( 1 ) );
			}
			vPortFree( dummy_buf );
		}
		else
		{
			mqtt_timeout = ( ( mqtt_timeout + MQTT_RECONNECT_DELAY_STEP_MS ) > MQTT_RECONNECT_DELAY_MAX_MS )
							? ( MQTT_RECONNECT_DELAY_MAX_MS )
							: ( mqtt_timeout + MQTT_RECONNECT_DELAY_STEP_MS );
		}

		FreeRTOS_closesocket( socket );

		mbedtls_ssl_free( ssl_ctx );
		vTaskDelay( pdMS_TO_TICKS( mqtt_timeout ) );
	}

	vPortFree( tls_ctx );
	vPortFree( ssl_ctx );
	vPortFree( ssl_conf );
	vPortFree( cert );
	vPortFree( prvkey );
	vPortFree( ca );
}

void mqtt_demo_create( rtos_gpio_t *gpio_ctx, UBaseType_t priority )
{
    if( gpio_ctx != NULL )
    {
        led_port = rtos_gpio_port(PORT_LEDS);

        rtos_gpio_port_enable(gpio_ctx, led_port);
        gpio = gpio_ctx;

        xTaskCreate( mqtt_demo_connect, "mqtt_demo", MQTT_DEMO_CONNECT_STACK_SIZE, ( void * ) NULL, priority, NULL );
    }
}
