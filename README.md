# AC9000

AC9000 is a machine that attaches a tape called ACF on FPC (Flexible Printed Circuit). Below is the process:

1. InPickArm will pick up a FPC from a tray and pass it to TA1.
2. TA1 will show it to a camera for alignment(2 point inspection).
3. TA1 will put FPC on ACF1WH/ACF2WH according to alignment.
4. InspOpt will inspect FPC on ACF1WH/ACF2WH for alignment(2 point inspection) again.
5. ACF1WH/ACF2WH will go to ACF1 for attachment.
6. InspOpt will inspect the attachment.
7. If attachment is successful, TA2 will pick up the FPC and pass it to the next machine, otherwise it will be thrown to a bin.
