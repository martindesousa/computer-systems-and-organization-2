#include "lab.h"
#include <string.h>
#include <stdio.h>

/* forward function for "attack 0" case.
   change this code to implement your attack
 */
void forward_attack_0(struct message *message) {
    strcpy((char*)message->data, "PAY $10001438 TO M");
    message->data_size = strlen("PAY $10001438 TO M");
    send_message(message);
    return;
}

/* forward function for "attack 1" case.
   change this code to implement your attack
 */
void forward_attack_1(struct message *message) {

    if (message->to == 'B') {
        char decrypted_data[1024];
        strcpy(decrypted_data, "PAY $10001438 TO M");
        struct message *modified_message = new_message('A','B', decrypted_data, true, false);
        send_message(modified_message);
        return;
    }
}

/* forward function for "attack 2" case.
   change this code to implement your attack
 */
void forward_attack_2(struct message *message) {
    static int replay_sent = 0;

    if (message->to == 'B')
    {
        if (replay_sent == 0)
        {
            send_message(message);
            replay_sent = 1;
        }
    }

    send_message(message);


}

/* forward function for "attack 3" case.
   change this code to implement your attack
 */
void forward_attack_3(struct message *message) {
    static int replay_sent = 0;
    static int counter = 0;

    if (counter == 2)
    {
        return;
    }
    if (message->to == 'B')
    {
        if (counter == 1)
        {
            if (replay_sent == 0)
            {
                send_message(message);
                replay_sent = 1;
            }
        }
        counter++;
    }


    send_message(message);

}

/* forward function for "attack 4" case.
   change this code to implement your attack */
void forward_attack_4(struct message *message) {
    send_message(message);
}

/* forward function for "attack 5" case.
   I did not intend this one to be possible. */
void forward_attack_5(struct message *message) {
    send_message(message);
}
