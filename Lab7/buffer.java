class Buffer {
	private int[] buf;
	private int inicio;
	private int fim;

	public Buffer(int size) {
		this.buf = new int[size];
		this.inicio = -1;
		this.fim = -1;

	}

	private boolean estaCheio(){
		return this.inicio == this.fim && this.inicio != -1;
	}

	private boolean estaVazio(){
		return this.inicio == this.fim && this.inicio == -1;
	}

	public synchronized void insere(int value) {
		try {
			System.out.println("Produtor(" +value+ "): querendo inserir");

			while ( this.estaCheio() ) {
				System.out.println("Produtor(" +value+ "): bloqueado");
				this.wait();
				System.out.println("Produtor(" +value+ "): notificado");

			}

			if (this.inicio == -1){
				this.inicio = 0;
				this.fim = 0;
			}

			this.buf[this.fim] = value;

			System.out.println("Produtor(" +value+ ") na posição " + this.fim + ": inseriu valor: " + value );

			this.fim = (this.fim + 1) % this.size();
		} catch (InterruptedException e) {
			System.out.println(e);
		} finally {
			this.notifyAll();
		}
	}

	public synchronized int pega(int id) {
		int value = 0;
		try {
			System.out.println("Consumidor(" +id+ "): querendo pegar");

			while ( this.estaVazio() ) {
				System.out.println("Consumidor(" +id+ "): bloqueado");
				this.wait();
				System.out.println("Consumidor(" +id+ "): notificado");
			}

			value = this.buf[this.inicio];

			System.out.println("Consumidor(" +id+ ") na posição " + this.inicio + ": pegou valor: " + value );

			this.inicio = (this.inicio + 1) % this.size();

			if (this.inicio == this.fim){
				this.inicio = this.fim = -1;
			}

		} catch (InterruptedException e) {
			System.out.println(e);
		} finally {
			this.notifyAll();
		}
		return value;
	}

	public int size() {
		return this.buf.length;
	}

	public synchronized void print() {
		System.out.print("Buffer: ");
		for (int i = 0; i < this.buf.length; i++) {
			if (this.estaVazio()){
				System.out.print("() ");
			}
			else if (this.inicio <= i || this.fim > i){
				System.out.print( this.buf[i] + " ");
			}
			else {
				System.out.print("() ");
			}
		}
		System.out.println();
	}
}
