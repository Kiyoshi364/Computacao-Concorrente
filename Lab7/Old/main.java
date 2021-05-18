class Consumidor extends Thread {

	private Buffer buf;
	private int id;

	public Consumidor(Buffer buf, int id) {
		this.buf = buf;
		this.id = id;
	}

	public void run() {
		while ( true ) {
			buf.pega(this.id);
      try {
			    sleep(500);
			} catch(InterruptedException e) {return; }
		}
	}
}

class Produtor extends Thread {
	private Buffer buf;
	private int id;

	public Produtor(Buffer buf, int id) {
		this.buf = buf;
		this.id = id;
	}

	public void run() {
		while ( true ) {
				buf.insere(this.id);
				try {
				    sleep(500);
				} catch(InterruptedException e) {return; }
		}
	}
}

class Main {
	private static final int L = 2;
	private static final int E = 2;
	private static final int TAM = 10;

	public static void main(String args[]) {
		int i;
		Buffer buf = new Buffer(TAM);
		Consumidor[] l = new Consumidor[L];  // Threads consumidoras
		Produtor[] e = new Produtor[E];      // Threads escritores

		for (i=0; i<L; i++) {
			l[i] = new Consumidor(buf, i+1);
			l[i].start();
		}
		for (i=0; i<E; i++) {
			e[i] = new Produtor(buf, i+1);
			e[i].start();
		}
	}
}
