#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <random>

using namespace std;

#define BUF_SIZE 100
#define MIN_VALUE 0
#define MAX_VALUE 99

int counterRead = 0;
int counterWrite = 0;
int counterDivide = 0;
int counterMerge = 0;

bool wasOutput = false;

struct Sequence {
    int data[10];
    int sum;
    Sequence() {
        sum = 0;
        for (int i = 0; i < 10; i++) {
            data[i] = 0;
        }
    }
    Sequence(int numbers[]) {
        sum = 0;
        for (int i = 0; i < 10; i++) {
            data[i] = numbers[i];
            sum += numbers[i];
        }
    }
    Sequence(int numbers[], int s) {
        sum = 0;
        for (int i = 0; i < 10; i++) {
            data[i] = numbers[i];
        }
        sum = s;
    }
};

struct File {
    const char* name;
    int posEnd;

    File(const char* n, int p) {
        name = n;
        posEnd = p;
    }

    void clearFile() {
        remove(name);
    }
};

struct ReadBuffer {
    Sequence* buf;
    int toRead; // sequence number to read from buffer
    int elements; // element number in buffer
    bool isEnd;
    File* f;

    ReadBuffer(File* File) {
        buf = new Sequence[BUF_SIZE];
        toRead = BUF_SIZE;
        elements = 0;
        isEnd = false;
        f = File;
    }

    // true - on sort, false - on copy
    Sequence* readSequence(bool isSum) {
        if (toRead == BUF_SIZE) {
            counterRead++;

            int array[10];
            int s;
            Sequence* seq;

            ifstream in(f->name);

            // go to the last read position
            in.seekg(f->posEnd, ios::beg);

            elements = 0;

            while ((elements < BUF_SIZE) && (!isEnd)) {
                if (isSum) {
                    if (in >> array[0] >> array[1] >> array[2] >> array[3] >> array[4] >> array[5] >> array[6] >> array[7] >> array[8] >> array[9] >> s) {
                        seq = new Sequence(array, s);
                        buf[elements] = *seq;
                        elements++;
                    }
                    else
                        isEnd = true;
                }
                if (!isSum) {
                    if (in >> array[0] >> array[1] >> array[2] >> array[3] >> array[4] >> array[5] >> array[6] >> array[7] >> array[8] >> array[9]) {
                        seq = new Sequence(array);
                        buf[elements] = *seq;

                        elements++;
                    }
                    else
                        isEnd = true;
                }
            }
            // save the position of the end of the read
            f->posEnd = in.tellg();
            in.close();

            toRead = 0;
        }
        toRead++;
        if (toRead > elements && isEnd) return NULL;
        return &(buf[toRead - 1]);
    }
};

struct WriteBuffer {
    Sequence* buf;
    int sequence;
    File* f;

    WriteBuffer(File* File) {
        buf = new Sequence[BUF_SIZE];
        sequence = 0;
        f = File;
        f->clearFile();
    }

    void printBuffer() {
        for (int i = 0; i < sequence; i++) {
            for (int j = 0; j < 10; j++)
                cout << "\t" << buf[i].data[j] << " ";
            cout << "\t\t" << buf[i].sum;
            cout << endl;
        }
        cout << endl;     
    }

    bool writeSequenceToFile(Sequence* seq, bool withSum) {
        if (sequence == BUF_SIZE) {
            counterWrite++;
            ofstream o(f->name, ios::out | ios::app);

            for (sequence = 0; sequence < BUF_SIZE; sequence++) {
                if (withSum) {
                    o << buf[sequence].data[0] << " " << buf[sequence].data[1] << " " << buf[sequence].data[2] << " " << buf[sequence].data[3] << " " << buf[sequence].data[4] << " " << buf[sequence].data[5] << " " << buf[sequence].data[6] << " " << buf[sequence].data[7] << " " << buf[sequence].data[8] << " " << buf[sequence].data[9] << " " << buf[sequence].sum << endl;
                }
                else {
                    o << buf[sequence].data[0] << " " << buf[sequence].data[1] << " " << buf[sequence].data[2] << " " << buf[sequence].data[3] << " " << buf[sequence].data[4] << " " << buf[sequence].data[5] << " " << buf[sequence].data[6] << " " << buf[sequence].data[7] << " " << buf[sequence].data[8] << " " << buf[sequence].data[9] << endl;
                }
            }
            o.close();
            sequence = 0;
        }
        if (seq == NULL) return false;
        buf[sequence] = *seq;
        sequence++;

        return true;
    }
    void writeRemain(bool withSum) {
        counterWrite++;
        ofstream o(f->name, ios::out | ios::app);
        for (int i = 0; i < sequence; i++) {
            if (withSum) {
                o << buf[i].data[0] << " " << buf[i].data[1] << " " << buf[i].data[2] << " " << buf[i].data[3] << " " << buf[i].data[4] << " " << buf[i].data[5] << " " << buf[i].data[6] << " " << buf[i].data[7] << " " << buf[i].data[8] << " " << buf[i].data[9] << " " << buf[i].sum << endl;
            }
            else {
                o << buf[i].data[0] << " " << buf[i].data[1] << " " << buf[i].data[2] << " " << buf[i].data[3] << " " << buf[i].data[4] << " " << buf[i].data[5] << " " << buf[i].data[6] << " " << buf[i].data[7] << " " << buf[i].data[8] << " " << buf[i].data[9] << endl;
            }
        }
        if (wasOutput)
            printBuffer();
        wasOutput = true;
        o.close();
        sequence = 0;
    }
    ~WriteBuffer() {
        writeRemain(true);
    }
};

void generate(char* filePath) {
    int data[10];
    int n;

    File* f = new File(filePath, 0);
    WriteBuffer* wb = new WriteBuffer(f);

    srand(time(NULL));

    cout << " Generate n sequences:" << endl;
    cout << " n = ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < 10; j++) {
            data[j] = rand() % MAX_VALUE;
        }
        wb->writeSequenceToFile(new Sequence(data), false);
    }
    wb->writeRemain(false);
}

void read(char* filePath) {
    int data[10];
    int n;

    File* f = new File(filePath, 0);
    WriteBuffer* wb = new WriteBuffer(f);

    cout << " n sequences to enter:" << endl;
    cout << " n = ";
    cin >> n;
    for (int i = 0; i < n; i++) {
        cout << " Enter sequence nr " << i + 1 << ":" << endl;
        for (int j = 0; j < 10; j++) {
            cin >> data[j];
        }
        wb->writeSequenceToFile(new Sequence(data), false);
    }
    wb->writeRemain(false);
}

void readFromFile(char* filePath) {
    int i;
    string str;
    cout << " Enter file name :" << endl;

    getchar();
    getline(cin, str);
    for (i = 0; i < str.length(); i++)
        filePath[i] = str[i];
    filePath[str.length()] = '\0';
}

void mode(char* filePath) {
    int c;

    cout << endl;
    cout << " 1. Generate sequences       " << endl;
    cout << " 2. Get sequences from file  " << endl;
    cout << " 3. Enter sequences          " << endl << endl;
    cout << " ";
    cin >> c;
    cout << endl;

    switch (c) {
    case 1:
        generate(filePath);
        break;
    case 2:
        readFromFile(filePath);
        break;
    case 3:
        read(filePath);
        break;
    }
}

void divide() {
    counterDivide++;
    cout << " Divide phase: " << counterDivide << endl;

    File* output1 = new File("a.txt", 0);
    File* output2 = new File("b.txt", 0);
    File* input = new File("c.txt", 0);

    ReadBuffer* rb = new ReadBuffer(input);
    WriteBuffer* wb1 = new WriteBuffer(output1);
    WriteBuffer* wb2 = new WriteBuffer(output2);

    Sequence* seq = NULL;
    int lastValue = 0;
    WriteBuffer* temp = wb1;

    while (true) {
        seq = rb->readSequence(true);
        if (seq == NULL) break;

        if (lastValue > seq->sum) {
            if (temp == wb1) temp = wb2;
            else temp = wb1;
        }
        temp->writeSequenceToFile(seq, true);
        lastValue = seq->sum;
    }
    delete wb1;
    delete wb2;
    delete rb;
}

bool merge() {
    File* input1 = new File("a.txt", 0);
    File* input2 = new File("b.txt", 0);
    File* output = new File("c.txt", 0);

    ReadBuffer* rb1 = new ReadBuffer(input1);
    ReadBuffer* rb2 = new ReadBuffer(input2);
    WriteBuffer* wb = new WriteBuffer(output);

    int prev1 = 0, prev2 = 0;
    Sequence* seq1 = rb1->readSequence(true);
    Sequence* seq2 = rb2->readSequence(true);

    if (seq2 == NULL) return false;
    counterMerge++;
    cout << " Merge phase: " << counterMerge << endl;

    while (true) {
        if ((seq1 != NULL) && (seq2 != NULL)) {
            if (seq1->sum < prev1) {
                while ((seq2 != NULL) && (seq2->sum > prev2)) {
                    wb->writeSequenceToFile(seq2, true);
                    prev2 = seq2->sum;
                    seq2 = rb2->readSequence(true);
                }
                prev1 = 0;
                prev2 = 0;
            }
            else if (seq2->sum < prev2) {
                while ((seq1 != NULL) && (seq1->sum > prev1)) {
                    wb->writeSequenceToFile(seq1, true);
                    prev1 = seq1->sum;
                    seq1 = rb1->readSequence(true);
                }
                prev1 = 0;
                prev2 = 0;
            }
            else {
                if (seq1->sum < seq2->sum) {
                    wb->writeSequenceToFile(seq1, true);
                    prev1 = seq1->sum;
                    seq1 = rb1->readSequence(true);
                }
                else {
                    wb->writeSequenceToFile(seq2, true);
                    prev2 = seq2->sum;
                    seq2 = rb2->readSequence(true);
                }
            }
        }
        else if (seq1 == NULL) {
            while (seq2 != NULL) {
                wb->writeSequenceToFile(seq2, true);
                seq2 = rb2->readSequence(true);
            }
            break;
        }
        else if (seq2 == NULL) {
            while (seq1 != NULL) {
                wb->writeSequenceToFile(seq1, true);
                seq1 = rb1->readSequence(true);
            }
            break;
        }
    }

    delete rb1;
    delete rb2;
    delete wb;

    return true;
}

void copyFile(const char* fileName1, const char* fileName2) {
    File* f1 = new File(fileName1, 0);
    File* f2 = new File(fileName2, 0);

    ReadBuffer* rb = new ReadBuffer(f1);
    WriteBuffer* wb = new WriteBuffer(f2);

    while (wb->writeSequenceToFile(rb->readSequence(false), true));
    delete rb;
    delete wb;
}
void writeFile() {
    File* f1 = new File("a.txt", 0);
    File* f2 = new File("output.txt", 0);

    ReadBuffer* rb = new ReadBuffer(f1);
    WriteBuffer* wb = new WriteBuffer(f2);

    cout << " Output:" << endl;
    while (wb->writeSequenceToFile(rb->readSequence(true), false));
    wb->writeRemain(false);

    remove("a.txt");
    remove("b.txt");
    delete rb;
    delete wb;
}

int main() {
    char filePath[100] = {
      "input.txt"
    };
    bool isSorted = true;

    mode(filePath);

    copyFile(filePath, "c.txt");
    while (isSorted) {
        divide();
        isSorted = merge();
    }
    writeFile();

   
    cout << " Phase counter: " << counterMerge << endl;
    cout << " Read counter: " << counterRead << endl;
    cout << " Write counter: " << counterWrite << endl << endl;

    return 0;
}
