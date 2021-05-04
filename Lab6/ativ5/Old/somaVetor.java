class Arr {
	int[] arr;

	public Arr(int[] arr) {
		this.arr = arr;
	}

	public boolean set(int index, int val) {
		if ( index >= this.arr.length ) return false;
		this.arr[index] = val;
		return true;
	}

	public int get(int index) {
		return this.arr[index];
	}

	public int size() {
		return this.arr.length;
	}

	public void print() {
		System.out.print("Array:");
		for ( int val : this.arr ) {
			System.out.print(" " + val);
		}
		System.out.println();
	}
}

class Adder implements Runnable {

	private Arr A;
	private Arr B;
	private Arr C;

	private int start;
	private int end;

	public Adder(Arr A, Arr B, Arr C, int start, int end) {
		this.A = A;
		this.B = B;
		this.C = C;
		this.start = start;
		this.end = end;
	}

	public void run() {
		for (int i = start; i < end; i++) {
			if ( !this.C.set(i, this.A.get(i) + this.B.get(i)) ) {
				System.out.println("Error on adding index " + i);
			}
		}
	}
}

class somaVetor {
	private static int N = 2;
	private static int ARR_LEN = 10;

	public static void main(String args[]) {
		Thread[] threads = new Thread[N];
		
		int[] a = new int[ARR_LEN];
		for (int i = 0; i < a.length; i++) {
			a[i] = i;
		}
		
		int[] b = new int[ARR_LEN];
		for (int i = 0; i < b.length; i++) {
			b[i] = ARR_LEN - i;
		}
		
		int[] c = new int[ARR_LEN];
		for (int i = 0; i < c.length; i++) {
			c[i] = 0;
		}
		
		Arr A = new Arr(a);
		Arr B = new Arr(b);
		Arr C = new Arr(c);
		
		int offset = 0;
		int len = ARR_LEN / N;
		int resto = ARR_LEN % N;

		//cria as threads da aplicacao
		for (int i=0; i<threads.length; i++) {
			int start = offset;
			int end = start + len;

			offset += len;
			if ( resto > 0 ) {
				end += 1;
				offset += 1;
				resto -= 1;
			}

			threads[i] = new Thread(new Adder(A, B, C, start, end));
		}

		//inicia as threads
		for (int i=0; i<threads.length; i++) {
			threads[i].start();
		}

		//espera pelo termino de todas as threads
		for (int i=0; i<threads.length; i++) {
			try { threads[i].join(); } catch (InterruptedException e) { return; }
		}

		C.print();
	}
}
