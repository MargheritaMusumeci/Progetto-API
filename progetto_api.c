/**
Progetto di API

PROGETTAZIONE:

Tutti i comandi vengono letti carattere per carattere e processati.
Tutte le righe vengono lette per intero.

Ogni comando letto viene processato come segue:

> c - il programma salva il comando e le righe che verranno sovrascritte in una lista dinamica.
       Dopodichè leggerà le nuove stringhe e le sovrascriverà nel testo;
> d - il programma salva le righe che verranno eliminate in una lista dinamica dopodichè si occuperà
			della loro eliminazione e quindi della modifica di tutto il testo;
> p - stampa le stringhe;
> u / r - il programma salva il numero di undo / redo da dover compiere dopodichè continua la lettura delle
      istruzioni. Eseguirà il comando quando verrà letta un istruzione diversa da r e u.
			Se viene letta la q il comando non verrà mai eseuito;
> q - il programma termina immediatamente.

Per rendere più efficiente le operationi di u e r ogni delete e ogni 39 change lo stato del testo viene salvato,
in modo da non dover annullare ogni operazione svolta per ricreare il testo.

STRUTTURA DATI:

riga --> Array[1025]
testo --> Array dinamico di puntatori ad Array[1025]
Lista di comandi eseguiti --> lista bidirezionale
*/

//librerie
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//costante che impone la massima dimensione della riga
#define MAX_RIGA 1025

//lista dinamica bidirezionale che tiene in memoria i comandi c e d eseguiti fino a quel momento
//per ogni comando di tipo c si salvano le righe che sarannosovrascritte
//per ogni comando di tipo d si salvano le righe che saranno eliminate
typedef struct elemento{
	struct elemento* precedente;
	int ind1;
	int ind2;
	//comando letto
	char com;
	//lunghezza del testo
	int massimo;
	char **stringavecchia;
	char **stringanuova;
	char **stato;
	//booleano per salvare lo stato del testo
	int salva;
	struct elemento *prossimo;
}comando;

//puntatore alla testa della lista di comandi eseguiti
comando *inizio=NULL;

//puntatore alla coda della lista di comandi eseguiti
comando *redo=NULL;

//indica quante redo posso essere ancora eseguite
int contaredo=0;

//indica quante undo posso essere ancora eseguite
int contaundo=0;

//punta a un array dinamico che ha in memoria la configurazione finale del testo
char **corrente=NULL;

//array dinamico che rappresenta l'intero testo
char *frase;

//array in cui viene salvata la riga letta
char riga[MAX_RIGA];

//indice che tiene in memoria la lunghezza attuale dell'array dinamico (il testo)
int max=0;

//booleano che serve a saperese lo stato deve essere aggiornato o meno
int salvaStato=0;

//salva l'insieme di comandi
void InserisciCD(int ind1,int ind2, char com){
	comando *nuovo;
	nuovo=malloc(sizeof(comando));
	nuovo->ind1=ind1;
	nuovo->ind2=ind2;
  nuovo->com=com;
	nuovo->massimo=max;
	nuovo->stringavecchia=NULL;
	nuovo->stringanuova=NULL;
	nuovo->stato=NULL;
	nuovo->prossimo=inizio;
	nuovo->precedente=NULL;
	nuovo->salva=0;
	if (inizio!=NULL)
	{inizio->precedente=nuovo;}
	inizio=nuovo;
	contaundo++;
	return;
}

//effettua la change
void Change(int ind1, int ind2){
	//i-> l'indice della riga attuale che sto scrivendo
	//j-> indice dell'array in cui salvo le stringhe
	int i,j;

	//max indica la dimensione attuale del testo
	//viene distinta la dimensione massima dell'array testo dalla dimensione di occupazione
	int occupate=max;
	int lenght;

	// se aggiungo in coda e devo aumentare la dimensione del vettore
	if (ind2>occupate){
		max=ind2;
		corrente=realloc(corrente,max*sizeof(char*));
	}

	i=ind1-1;
	j=0;

	//genera lo spazio di memoria necessario a salvare le righe di testo
	inizio->stringavecchia=calloc((ind2-ind1+1),sizeof(char*));
	inizio->stringanuova=calloc((ind2-ind1+1),sizeof(char*));

	//finchè ci sono righe da scrivere
	while(i<(ind2)){
		//legge la riga
		fgets(riga,MAX_RIGA,stdin);

		//calcola la sua lunghezza
		lenght=strlen(riga);

		//alloca la zona di memoria necessaria a salvare la stringa
		frase=malloc((lenght+1)*sizeof(char));

		//copia in frase la riga appena letta
		strcpy(frase,riga);

		//salva quello che c'è nel testo all'indice che devo andare a sovrascrivere
		if(i<occupate){
			inizio->stringavecchia[j]=corrente[i];
		}
		inizio->stringanuova[j]=frase;
		corrente[i]=frase;
		i++;
		j++;
	}


	//ogni 39 operazioni di change salva lo stato del testo
	if (salvaStato%39==0){

		if (corrente!=NULL){
			inizio->massimo=max;
			inizio->salva=1;
			inizio->stato=calloc(max,sizeof(char*));
			i=0;
			while(i<max){
				inizio->stato[i]=corrente[i];
				i++;
			}
		}
		//testo vuoto
		else{
			inizio->massimo=0;
			inizio->salva=1;
			inizio->stato=NULL;
		}
	}
	salvaStato++;
	return;
	}

//effettua la delete delle righe
void Delete(int ind1,int ind2){
	int i,j;

	//salva in memoria lo stato finale
	if (corrente!=NULL){
		inizio->salva=1;
		inizio->stato=calloc(max,sizeof(char*));
		i=0;
		while(i<max){
			inizio->stato[i]=corrente[i];
			i++;
		}
	}
	else{
			inizio->massimo=0;
			inizio->salva=1;
			inizio->stato=NULL;
	}

	//caso (0,x)d
	if (ind1==0){
		ind1++;
	}

	//elimina più righe di quante sono presenti
	if (((ind2-ind1+1)>=max)&&(ind1==1)){
		inizio->stringavecchia=inizio->stato;
		corrente=NULL;
		max=0;
	}

	//0,0d non fa nulla
	//ind1>max elimino righe che non esistono
	//if ((ind1>max)||(ind2==0)) non fa niente
	if ((ind1<=(max))&&(ind2!=0)&&(max!=0)){
		i=ind1-1;
		j=0;

		//salva le righe che sto andando a eliminare
		inizio->stringavecchia=calloc((ind2-ind1+1),sizeof(char*));
		while((i<ind2)&&(i<max)){
			inizio->stringavecchia[j]=corrente[i];
			i++;
			j++;
		}

		//sovrascrive
		i=ind1-1;
		j=ind2;
		while (j<max){
			corrente[i]=corrente[j];
			corrente[j]=NULL;
			i++;
			j++;
		}

		if (ind2<max){
			max=max-(ind2-ind1+1);
		}
		else{
			max=ind1;
		}
	}
	return;
}

//stampa il testo richiesto
void Print(int ind1,int ind2){
	if (ind1==0){
		fputs(".\n",stdout);
		ind1++;
	}
	ind1--;
	while ((ind1<ind2)&&(ind1<max)){
		fputs(corrente[ind1],stdout);
		ind1++;
	}

	//per ogni riga vuota stampo un .
	while(ind1<ind2){
		fputs(".\n",stdout);
		ind1++;
	}
	return;
}

//annulla le operazioni
void Undo(int contatore){
	comando *temporaneo;
	comando *ultimadelete=NULL;
	int i,j,k,cont;
	cont=0;

	//controlla se tra le operazioni che devo annullare c'è almeno una di cui ho salvato lo stato
	i=contatore;
	temporaneo=inizio;
	while ((i>0)&&(temporaneo!=NULL)){
		if (temporaneo->salva==1){
			ultimadelete=temporaneo;
		}
		temporaneo=temporaneo->prossimo;
		i--;
	}
	redo=inizio;

	//se ha trovato uno stato salvato sposta i comandi precedenti senza rifare le operazioni al contrario
	if (ultimadelete!=NULL){
		while ((inizio!=ultimadelete)&&(inizio!=NULL)){
			redo=inizio;
			inizio=inizio->prossimo;
			contatore--;
		}

		//esce quando inizio e ultimadelete combaciano
		max=inizio->massimo;
		if (inizio->stato!=NULL){
			corrente=calloc(max,sizeof(char*));
			i=0;
			while(i<max){
				corrente[i]=inizio->stato[i];
				i++;
			}
		}
		else{
			corrente=NULL;
			max=0;
		}

		if (inizio->com=='d'){
			redo=inizio;
			inizio=inizio->prossimo;
			contatore--;
		}
	}
	//tutte le funzioni che rimangono sono delle change da dover svolgere al contrario
	while((contatore>0)&&(inizio!=NULL)){
		i=0;
		while ((i<((inizio->ind2)-(inizio->ind1)+1))&&(inizio->stringavecchia!=NULL)&&(inizio->stringavecchia[i]!=NULL)){
			cont++;
			i++;
		}
		//vuol dire che con la change attuale ha semplicemente aggiunto righe in coda
		if (cont==0){
			max = max-((inizio->ind2)-(inizio->ind1)+1);
			if (max<=0){
				corrente=NULL;
				max=0;
			}
			else{
				corrente=realloc(corrente,max*sizeof(char*));
			}
		}
		else{
			//vuol dire che ha in parte sovrascritto e in parte aggiunto in coda
			if (cont<((inizio->ind2)-(inizio->ind1)+1)){
				i=inizio->ind1-1;
				j=inizio->ind1+cont;
				k=0;
				while(i<j){
					corrente[i]=inizio->stringavecchia[k];
					k++;
					i++;
				}
				max=max-((inizio->ind2)-(inizio->ind1)+1-cont);
				if (max<=0){
					corrente=NULL;
					max=0;
				}
				else{
					corrente=realloc(corrente,max*sizeof(char*));
				}
			}
			//ha totalmente sovrascritto
			if (cont==(inizio->ind2-inizio->ind1+1)){
				i=inizio->ind1-1;
				j=0;
				while(i<inizio->ind2){
					corrente[i]=inizio->stringavecchia[j];
					i++;
					j++;
				}
			}
		}
		redo=inizio;
		inizio=inizio->prossimo;
		contatore--;
		cont=0;
	}
	return;
}

//riprende le operazioni
void Redo(int contatore){
	comando *temporaneo;
	comando *ultimadelete=NULL;
	int i,j,k,occupate,lenght,cont;
	cont=0;

	//controlla se tra le operazioni che deve annullare c'è almeno una delete
	i=contatore;
	if ((redo!=NULL)){
		temporaneo=redo;
		while ((i>0)&&(temporaneo!=NULL)){
			if (temporaneo->salva==1){
				ultimadelete=temporaneo;
			}
			temporaneo=temporaneo->precedente;
			i--;
		}
	}

	//se ha trovato uno stato salvato sposto i comandi precedenti senza rifare le operazioni al contrario
	if (ultimadelete!=NULL){
		while ((inizio!=ultimadelete)&&(redo!=NULL)){
			if (inizio!=NULL){
				inizio=inizio->precedente;
				redo=inizio->precedente;
			}
			else{
				inizio=redo;
			}
			contatore--;
		}
		//esce quando redo e ultimadelete combaciano
		max=ultimadelete->massimo;
		if (inizio->stato!=NULL){
			corrente=calloc(max,sizeof(char*));
			i=0;
			while(i<max){
				corrente[i]=inizio->stato[i];
				i++;
			}
		}
		else{
			corrente=NULL;
		}
		if (inizio->com=='d'){
			Delete(inizio->ind1,inizio->ind2);
		}
	}

	if ((contatore>0)&&(inizio!=NULL)){
		inizio=inizio->precedente;
		if (inizio!=NULL){
			redo=inizio->precedente;
		}
	}
	else{
		if (contatore>0){
			inizio=redo;
		}
	}

	//le ultime redo che deve fare sono tutte change
	while(contatore>0){
		occupate=max;
			//esegue la change leggendo le righe da stringanuova

			//aggiunge in coda allora crea un vettore più grande
			if (inizio->ind2>max){
				max=inizio->ind2;
				corrente=realloc(corrente,max*sizeof(char*));
			}
			else{
				if (inizio->ind2>occupate){
					max=inizio->ind2;
				}
			}

			i=inizio->ind1-1;
			j=0;

			while(i<(inizio->ind2)&&(inizio->stringavecchia!=NULL)){
				//salva quello che c'era prima
				if(i<occupate){
					inizio->stringavecchia[j]=corrente[i];
				}
				else{
					inizio->stringavecchia[j]=NULL;
				}
				corrente[i]=inizio->stringanuova[j];
				i++;
				j++;
			}
		contatore--;
		if (contatore>0){
			inizio=inizio->precedente;
			if (inizio!=NULL){
				redo=inizio->precedente;
			}
		}
	}
	return;
}

int main(){

	//variabile dove viene salvato il comando letto
	char comandoLetto;

	//indice 1
	int ind1;

	//indice 2
	int ind2;

	//copia dell'indice 1
	char ind1char[6];

	//copia dell'indice 2
	char ind2char[6];

	//indice nella lettura dei caratteri dei comandi
	int i;

	//salva il numero di u e r da svolgere
	int contatoreUR;

	contatoreUR=0;

	//legge un carattere
	comandoLetto=fgetc(stdin);

	//continua le operazioni fino al carattere terminatore 'q'
	while (comandoLetto!='q'){
		i=0;
		while((comandoLetto!=',')&&(comandoLetto!='u')&&(comandoLetto!='r')){

			//legge il primo indice
			ind1char[i]=comandoLetto;
			i++;
			comandoLetto=fgetc(stdin);
		}

		//funzione che trasforma un char nel rispettivo int
		ind1=atoi(ind1char);

		if ((comandoLetto!='u')&&(comandoLetto!='r')){

			comandoLetto=fgetc(stdin);
			i=0;
			while ((comandoLetto!='c')&&(comandoLetto!='p')&&(comandoLetto!='d')){
				//legge il secondo indice
				ind2char[i]=comandoLetto;
				i++;
				comandoLetto=fgetc(stdin);
			}
			ind2=atoi(ind2char);

			//esegue l'operazione di change
			if (comandoLetto=='c'){

				//legge la riga
				fgets(riga,MAX_RIGA,stdin);

				//salva il nuovo comando che sta per essere eseguito
				InserisciCD(ind1,ind2,'c');

				//esegue il comando
				Change(ind1,ind2);

				//va avanti
				fgets(riga,MAX_RIGA,stdin);
			}

			//esegue l'operazione di delete
			if (comandoLetto=='d'){

				//salva il nuovo comando che sta per essere eseguito
				InserisciCD(ind1,ind2,'d');

				//esegue il comando
				Delete(ind1,ind2);

				//va avanti
				fgets(riga,MAX_RIGA,stdin);
			}

			//esegue l'operazione di print
			if (comandoLetto=='p'){

				//esegue il comando
				Print(ind1,ind2);

				//va avanti
				fgets(riga,MAX_RIGA,stdin);
			}
		}
		else{
			if ((comandoLetto=='u')||((comandoLetto=='r')&&(redo!=NULL))){


				//salva il numero di undo che andranno svolte
				if (comandoLetto=='u'){

					//se le operazioni eseguite sono maggiori delle operazioni che si vogliono annullare
					if (ind1<contaundo){

						//incrementa il numero di undo da dover svolgere
						contatoreUR=contatoreUR+ind1;

						//il numero di redo che si possono esegure aumenta
						contaredo=contaredo+ind1;

						//il numero di undo che si possono eseguire diminuisce
						contaundo=contaundo-ind1;
					}
					//viene chiesto di effettuare più undo di quanti sono i comandi
					else{

						//incrementa il numero di undo da dover svolgere
						contatoreUR=contatoreUR+contaundo;

						//il numero di redo che si possono esegure aumenta
						contaredo=contaredo+contaundo;

					 //il numero di undo che si possono esegure viene azzerato
						contaundo=0;
					}
				}
				else{

					//se le operazioni annullate sono maggiori delle operazioni che si vogliono riprendere
					if (ind1<contaredo){

							//diminuisce il numero di undo da dover svolgere
							contatoreUR=contatoreUR-ind1;

							//aumenta il numero di undo da poter svolgere
							contaundo=contaundo+ind1;

							//diminusice il numero di redo da poter svolgere
							contaredo=contaredo-ind1;
						}

						//se le operazioni annullate sono minori delle operazioni che si vogliono riprendere
						else{

							//diminuisce il numero di undo da dover svolgere
							contatoreUR=contatoreUR-contaredo;

							//aumenta il numero di undo da poter svolgere
							contaundo=contaundo+contaredo;

							//azzera il numero di redo da poter svolgere
							contaredo=0;
						}
				}

				//reset degli array necessario quando si leggono numeri molto grandi
				for (i=0;i<6;i++)
				{
					ind1char[i]=0;
					ind2char[i]=0;
				}

				//comado per eliminare il \n
				comandoLetto=fgetc(stdin);
				comandoLetto=fgetc(stdin);

				//le undo e le redo non vengono immediatamente effettuate
				//comandi adiacenti di r e u vengono accorpati tra loro
				while((comandoLetto!='q')&&(comandoLetto!=',')){
					i=0;
					while((comandoLetto!=',')&&(comandoLetto!='u')&&(comandoLetto!='r')){
						//legge il primo numero
						ind1char[i]=comandoLetto;
						i++;
						comandoLetto=fgetc(stdin);
					}
					ind1=atoi(ind1char);

					//come sopra
					if (comandoLetto=='u'){
						if (ind1<contaundo){
							contatoreUR=contatoreUR+ind1;
							contaredo=contaredo+ind1;
							contaundo=contaundo-ind1;
						}
						else{
							contatoreUR=contatoreUR+contaundo;
							contaredo=contaredo+contaundo;
							contaundo=0;
						}

						//reset degli array
						for (i=0;i<6;i++){
							ind1char[i]=0;
							ind2char[i]=0;
						}
						comandoLetto=fgetc(stdin); //\n
						comandoLetto=fgetc(stdin);
					}
					if (comandoLetto=='r'){
						if (ind1<contaredo){
							contatoreUR=contatoreUR-ind1;
							contaundo=contaundo+ind1;
							contaredo=contaredo-ind1;
						}
						else{
							contatoreUR=contatoreUR-contaredo;
							contaundo=contaundo+contaredo;
							contaredo=0;
						}

						for (i=0;i<6;i++){
							ind1char[i]=0;
							ind2char[i]=0;
						}
						comandoLetto=fgetc(stdin); //\n
						comandoLetto=fgetc(stdin);
					}
				}

				//se esce perchè è presente il carattere terminatore
				if (comandoLetto=='q'){
					//il programma termina immediatamente
					return 0;
				}

				if((contatoreUR>0)&&(inizio!=NULL)){

					//effettua le undo
					Undo(contatoreUR);

					//azzera il contatore di operazioni da svolgere
					contatoreUR=0;
				}
				//contatore<0 effettuo le redo
				else {
					if (redo!=NULL){

						//effettuo le redo
						Redo(-contatoreUR);
						//azzera il contatore di operazioni da svolgere
						contatoreUR=0;
					}
				}

				comandoLetto=fgetc(stdin);
				i=0;

				while ((comandoLetto!='c')&&(comandoLetto!='p')&&(comandoLetto!='d')){
					//legge il secondo numero
					ind2char[i]=comandoLetto;
					i++;
					comandoLetto=fgetc(stdin);
				}
				ind2=atoi(ind2char);

			//elimina le modifiche che sono state annullate
				if (comandoLetto=='c'){
					redo=NULL;
					if (inizio!=NULL){
						inizio->precedente=NULL;
					}
					contaredo=0;
					fgets(riga,MAX_RIGA,stdin);
					InserisciCD(ind1,ind2,'c');
					Change(ind1,ind2);
					fgets(riga,MAX_RIGA,stdin);
				}
				if (comandoLetto=='d'){
					redo=NULL;
					if (inizio!=NULL){
						inizio->precedente=NULL;
					}
					contaredo=0;
					InserisciCD(ind1,ind2,'d');
					Delete(ind1,ind2);
					fgets(riga,MAX_RIGA,stdin);
				}
				//non va azzerato il registro vecchio
				if (comandoLetto=='p'){
					Print(ind1,ind2);
					fgets(riga,MAX_RIGA,stdin);
				}
			}
		}
		//azzero gli array
		for (i=0;i<6;i++){
			ind1char[i]=0;
			ind2char[i]=0;
		}
		if (comandoLetto!='q'){
			comandoLetto=fgetc(stdin);
			if (comandoLetto=='\n'){
				comandoLetto=fgetc(stdin);
			}
		}
		else{
			return 0;
		}
	}
	return 0;
}
