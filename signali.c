#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>


struct timespec t0; /* vrijeme pocetka programa */

/* postavlja trenutno vrijeme u t0 */
void postavi_pocetno_vrijeme()
{
	clock_gettime(CLOCK_REALTIME, &t0);
}

/* dohvaca vrijeme proteklo od pokretanja programa */
void vrijeme(void)
{
	struct timespec t;

	clock_gettime(CLOCK_REALTIME, &t);

	t.tv_sec -= t0.tv_sec;
	t.tv_nsec -= t0.tv_nsec;
	if (t.tv_nsec < 0) {
		t.tv_nsec += 1000000000;
		t.tv_sec--;
	}

	printf("%03ld.%03ld:\t", t.tv_sec, t.tv_nsec/1000000);
}

/* ispis kao i printf uz dodatak trenutnog vremena na pocetku */
#define PRINTF(format, ...)       \
do {                              \
  vrijeme();                      \
  printf(format, ##__VA_ARGS__);  \
}                                 \
while(0)

/*
 * spava zadani broj sekundi
 * ako se prekine signalom, kasnije nastavlja spavati neprospavano
 */
void spavaj(time_t sekundi)
{
	struct timespec koliko;
	koliko.tv_sec = sekundi;
	koliko.tv_nsec = 0;

	while (nanosleep(&koliko, &koliko) == -1 && errno == EINTR);
		//PRINTF("Bio prekinut, nastavljam\n");
}

int sp = -1;
int stog[3] = {0,0,0};
int K_Z[3] = {0,0,0};
int T_P = 0;
int printaj[3]= {1, 1, 1};

void printaj_stog()
{
	if (sp == -1)
	{
		printf(" - ");
		return;
	}
	for (int i = sp; i >= 0; i--)
	{
		int n = stog[i];
		printf(" %d reg[%d]", n, n);
	}
}

void ispis()
{
	int broj = K_Z[0] * 100 + K_Z[1] * 10 + K_Z[2];
	PRINTF("K_Z=%03d, T_P=%d, stog:", broj, T_P);
	printaj_stog();
	printf("\n");

}

int postoji_signal()
{
    int postoji = -1;
    
    if (K_Z[2])
        postoji = SIGINT;
    else if (K_Z[1])
        postoji = SIGTERM;
    else if (K_Z[0])
        postoji = SIGUSR1;

    return postoji;
}

void obradi_signal(int sig)
{
    
    int prioritet = 0;
    if (sig == SIGINT)
    {
        prioritet = 3;
    }
    if (sig == SIGTERM)
    {
        prioritet = 2;
    }
    if (sig == SIGUSR1)
    {
        prioritet = 1;
    }

    //f (prioritet1(postoji_signal()) > prioritet) obradi_signal()
    if (prioritet == T_P)
    {
        PRINTF("SKLOP: Dogodio se prekid razine %d ali se on pamti i ne prosljeđuje procesoru\n", prioritet);   
        K_Z[prioritet - 1] = 1;
        ispis();
        return;
    }

    if (prioritet < T_P)
    {
        if (printaj[prioritet - 1] == 1)
        {
            PRINTF("SKLOP: Dogodio se prekid razine %d ali se on pamti i ne prosljeđuje procesoru\n", prioritet);
            printaj[prioritet - 1] = 0;
        }    
        K_Z[prioritet - 1] = 1;
        ispis();
        return;
    }
    K_Z[prioritet - 1] = 1;
    if (printaj[prioritet - 1] == 1 || prioritet == T_P)
        {
            PRINTF("SKOLP: Dogodio se prekid razine %d i prosljeđuje se procesoru\n", prioritet);
            printaj[prioritet - 1] = 0;
        } 
    ispis();
    K_Z[prioritet - 1] = 0;
    stog[++sp] = T_P;
    T_P = prioritet;
    PRINTF("Počela obrada razine %d\n", prioritet);
    ispis();
    for (int i = 0; i < 10; i++)
    {
        spavaj(1);
    }
    PRINTF("Završila obrada prekida razine %d\n", prioritet);
    //int trenutno = T_P;
    T_P = stog[sp--];
    if (T_P != 0)
    {
        PRINTF("Nastavlja se obrada prekida razine %d\n", T_P);
        ispis();
    }
    if (T_P == 0)
    {
        PRINTF("Nastavlja se izvođenje glavnog programa\n");
        ispis();
    }
    //PRINTF("T_P = %d\n", T_P);

    int novi_sig = postoji_signal();
    if (novi_sig != -1)
    {
        int tmp = 0;
        if (novi_sig == SIGINT) tmp = 3;
        if (novi_sig == SIGTERM) tmp = 2;
        if (novi_sig == SIGUSR1) tmp = 1;
        if (T_P == 0)
        {
            PRINTF("SKLOP: promijenio se T_P, prosljeđuje se prekid razine %d procesoru\n", tmp);
        }
        obradi_signal(novi_sig);
    }

    printaj[prioritet - 1] = 1; 
    return;
}

void inicijalizacija()
{
	struct sigaction act;

    // SIGINT
	act.sa_handler = obradi_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_NODEFER;
	sigaction(SIGINT, &act, NULL);

    // SIGTERM
    act.sa_handler = obradi_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGTERM, &act, NULL);

    // SIGUSR1
    act.sa_handler = obradi_signal;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	sigaction(SIGUSR1, &act, NULL);



	postavi_pocetno_vrijeme();
}

int main()
{

    PRINTF("Program s PID=%d krenuo s radom\n", getpid());
    PRINTF("K_Z=000, T_P=0, stog: -\n");

	inicijalizacija();
	while (1)
    {
        sleep(1);
    };

	return 0;
}
