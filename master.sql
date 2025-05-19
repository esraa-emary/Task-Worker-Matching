/*==============================================================*/
/* DBMS name:      Microsoft SQL Server 2005                    */
/* Created on:     5/10/2025 4:12:54 PM                         */
/*==============================================================*/
CREATE DATABASE TaskWorkerMatching;
USE TaskWorkerMatching;

/* Foreign key constraint drops */
if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('ASSIGNMENT') and o.name = 'FK_ASSIGNME_HASASSIGN_CLIENT')
alter table ASSIGNMENT
drop constraint FK_ASSIGNME_HASASSIGN_CLIENT
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('ASSIGNMENT') and o.name = 'FK_ASSIGNME_ISASSIGNE_REQUEST')
alter table ASSIGNMENT
drop constraint FK_ASSIGNME_ISASSIGNE_REQUEST
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('ASSIGNMENT') and o.name = 'FK_ASSIGNME_PERFORMS_WORKER')
alter table ASSIGNMENT
drop constraint FK_ASSIGNME_PERFORMS_WORKER
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('REQUEST') and o.name = 'FK_REQUEST_MAKES_CLIENT')
alter table REQUEST
drop constraint FK_REQUEST_MAKES_CLIENT
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('REQUEST') and o.name = 'FK_REQUEST_REQUIRES_TASK')
alter table REQUEST
drop constraint FK_REQUEST_REQUIRES_TASK
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('SPECIALTY') and o.name = 'FK_SPECIALT_CAN_DO_WORKER')
alter table SPECIALTY
drop constraint FK_SPECIALT_CAN_DO_WORKER
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('SPECIALTY') and o.name = 'FK_SPECIALT_REFERENCE_TASK')
alter table SPECIALTY
drop constraint FK_SPECIALT_REFERENCE_TASK
go

if exists (select 1
   from sys.sysreferences r join sys.sysobjects o on (o.id = r.constid and o.type = 'F')
   where r.fkeyid = object_id('WORKER') and o.name = 'FK_WORKER_ASSIGNED__REQUEST')
alter table WORKER
drop constraint FK_WORKER_ASSIGNED__REQUEST
go

/* Index drops */
if exists (select 1
            from  sysindexes
           where  id    = object_id('ASSIGNMENT')
            and   name  = 'HASASSIGNMENT_FK'
            and   indid > 0
            and   indid < 255)
drop index ASSIGNMENT.HASASSIGNMENT_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('ASSIGNMENT')
            and   name  = 'PERFORMS_FK'
            and   indid > 0
            and   indid < 255)
drop index ASSIGNMENT.PERFORMS_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('ASSIGNMENT')
            and   name  = 'ISASSIGNED2_FK'
            and   indid > 0
            and   indid < 255)
drop index ASSIGNMENT.ISASSIGNED2_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('REQUEST')
            and   name  = 'REQUIRES_FK'
            and   indid > 0
            and   indid < 255)
drop index REQUEST.REQUIRES_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('REQUEST')
            and   name  = 'MAKES_FK'
            and   indid > 0
            and   indid < 255)
drop index REQUEST.MAKES_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('SPECIALTY')
            and   name  = 'CAN_DO_FK'
            and   indid > 0
            and   indid < 255)
drop index SPECIALTY.CAN_DO_FK
    go

if exists (select 1
            from  sysindexes
           where  id    = object_id('WORKER')
            and   name  = 'ASSIGNED_TO_FK'
            and   indid > 0
            and   indid < 255)
drop index WORKER.ASSIGNED_TO_FK
    go

/* Table drops */
if exists (select 1
            from  sysobjects
           where  id = object_id('ASSIGNMENT')
            and   type = 'U')
drop table ASSIGNMENT
    go

    if exists (select 1
    from  sysobjects
    where  id = object_id('CLIENT')
    and   type = 'U')
drop table CLIENT
    go

    if exists (select 1
    from  sysobjects
    where  id = object_id('REQUEST')
    and   type = 'U')
drop table REQUEST
    go

    if exists (select 1
    from  sysobjects
    where  id = object_id('SPECIALTY')
    and   type = 'U')
drop table SPECIALTY
    go

    if exists (select 1
    from  sysobjects
    where  id = object_id('TASK')
    and   type = 'U')
drop table TASK
    go

    if exists (select 1
    from  sysobjects
    where  id = object_id('WORKER')
    and   type = 'U')
drop table WORKER
    go

/* Tables */

/*==============================================================*/
/* Table: ASSIGNMENT                                            */
/*==============================================================*/
create table ASSIGNMENT (
                            WORKERID             int                  not null,
                            CLIENTID             int                  not null,
                            REQUESTID            int                  not null,
                            ACTUALTIMETAKEN      int                  null,
                            WORKERRATING         float                null,
                            CLIENTRATING         float                null,
                            STATUS               varchar(100)         null,
                            FEEDBACKBYCLIENT     text                 null,
                            FEEDBACKBYWORKER     text                 null,
                            WAGE                 DECIMAL(10, 2)       null 
)
    go

/*==============================================================*/
/* Table: CLIENT                                                */
/*==============================================================*/
create table CLIENT (
                        CLIENTID             int                  not null,
                        NAME                 varchar(100)         not null,
                        PHONE                varchar(100)         not null,
                        ADDRESS              varchar(250)         null,
                        EMAIL                varchar(100)         not null,
                        OVERALLFEEDBACK      varchar(1024)        null,
                        PASSWORD             varchar(100)         not null,
                        constraint PK_CLIENT primary key nonclustered (CLIENTID)
)
    go

/*==============================================================*/
/* Table: REQUEST                                               */
/*==============================================================*/
create table REQUEST (
                         REQUESTID            int                  not null,
                         CLIENTID             int                  not null,
                         TASKID               int                  not null,
                         ADDRESS              varchar(250)         not null,
                         REQUESTTIME          datetime            null,
                         PREFERREDTIMESLOT    datetime             null,
                         REQUESTDESCRIPTION   varchar(250)         null,
                         constraint PK_REQUEST primary key nonclustered (REQUESTID)
)
    go

/*==============================================================*/
/* Table: SPECIALTY                                             */
/*==============================================================*/
create table SPECIALTY (
                           WORKERID             int                  null,
                           TASKID               int                  null
)
    go

/*==============================================================*/
/* Table: TASK                                                  */
/*==============================================================*/
create table TASK (
                      TASKID               int                  not null,
                      TASKNAME             varchar(100)         not null,
                      AVGTIMETOFINISH      int                  null,
                      AVGFEE               float                null,
                      constraint PK_TASK primary key nonclustered (TASKID)
)
    go

/*==============================================================*/
/* Table: WORKER                                                */
/*==============================================================*/
create table WORKER (
                        WORKERID             int                  not null,
                        REQUESTID            int                  null,
                        NAME                 varchar(100)         not null,
                        SPECIALTYNUM         int                  not null,
                        LOCATIONS            varchar(250)         null,
                        AVAILABLESLOTS       datetime             null,
                        RATING               float                null,
                        constraint PK_WORKER primary key nonclustered (WORKERID)
)
    go

/* All foreign key constraint adds */

alter table ASSIGNMENT
    add constraint FK_ASSIGNME_HASASSIGN_CLIENT foreign key (CLIENTID)
        references CLIENT (CLIENTID)
    go

alter table ASSIGNMENT
    add constraint FK_ASSIGNME_ISASSIGNE_REQUEST foreign key (REQUESTID)
        references REQUEST (REQUESTID)
    go

alter table ASSIGNMENT
    add constraint FK_ASSIGNME_PERFORMS_WORKER foreign key (WORKERID)
        references WORKER (WORKERID)
    go

alter table REQUEST
    add constraint FK_REQUEST_MAKES_CLIENT foreign key (CLIENTID)
        references CLIENT (CLIENTID)
    go

alter table REQUEST
    add constraint FK_REQUEST_REQUIRES_TASK foreign key (TASKID)
        references TASK (TASKID)
    go

alter table SPECIALTY
    add constraint FK_SPECIALT_CAN_DO_WORKER foreign key (WORKERID)
        references WORKER (WORKERID)
    go

alter table SPECIALTY
    add constraint FK_SPECIALT_REFERENCE_TASK foreign key (TASKID)
        references TASK (TASKID)
    go

alter table WORKER
    add constraint FK_WORKER_ASSIGNED__REQUEST foreign key (REQUESTID)
        references REQUEST (REQUESTID)
    go

/* index creations */

/*==============================================================*/
/* Index: ISASSIGNED2_FK                                        */
/*==============================================================*/
create index ISASSIGNED2_FK on ASSIGNMENT (
                                           REQUESTID ASC
    )
    go

/*==============================================================*/
/* Index: PERFORMS_FK                                           */
/*==============================================================*/
create index PERFORMS_FK on ASSIGNMENT (
                                        WORKERID ASC
    )
    go

/*==============================================================*/
/* Index: HASASSIGNMENT_FK                                      */
/*==============================================================*/
create index HASASSIGNMENT_FK on ASSIGNMENT (
                                             CLIENTID ASC
    )
    go

/*==============================================================*/
/* Index: MAKES_FK                                              */
/*==============================================================*/
create index MAKES_FK on REQUEST (
                                  CLIENTID ASC
    )
    go

/*==============================================================*/
/* Index: REQUIRES_FK                                           */
/*==============================================================*/
create index REQUIRES_FK on REQUEST (
                                     TASKID ASC
    )
    go

/*==============================================================*/
/* Index: CAN_DO_FK                                             */
/*==============================================================*/
create index CAN_DO_FK on SPECIALTY (
                                     WORKERID ASC
    )
    go

/*==============================================================*/
/* Index: ASSIGNED_TO_FK                                        */
/*==============================================================*/
create index ASSIGNED_TO_FK on WORKER (
                                       REQUESTID ASC
    )
    go


/* CRUD Procedures */

-- Client Procedures
CREATE PROCEDURE CreateClient
    @ClientID INT,
    @Name VARCHAR(100),
    @Phone VARCHAR(100),
    @Address VARCHAR(250),
    @Email VARCHAR(100),
    @Password VARCHAR(100)
AS
BEGIN
INSERT INTO CLIENT (CLIENTID, NAME, PHONE, ADDRESS, EMAIL, PASSWORD)
VALUES (@ClientID, @Name, @Phone, @Address, @Email, @Password)
END
GO

CREATE PROCEDURE UpdateClient
    @ClientID INT,
    @Name VARCHAR(100),
    @Phone VARCHAR(100),
    @Address VARCHAR(250),
    @Email VARCHAR(100),
    @Password VARCHAR(100)
AS
BEGIN
UPDATE CLIENT
SET NAME = @Name,
    PHONE = @Phone,
    ADDRESS = @Address,
    EMAIL = @Email,
    PASSWORD = @Password
WHERE CLIENTID = @ClientID
END
GO

CREATE PROCEDURE DeleteClient
    @ClientID INT
AS
BEGIN
DELETE FROM CLIENT WHERE CLIENTID = @ClientID
END
GO

CREATE PROCEDURE GetClient
    @ClientID INT
AS
BEGIN
SELECT * FROM CLIENT WHERE CLIENTID = @ClientID
END
GO

CREATE PROCEDURE GetAllClients
    AS
BEGIN
SELECT * FROM CLIENT
END
GO

-- Worker Procedures
CREATE PROCEDURE CreateWorker
    @WorkerID INT,
    @Name VARCHAR(100),
    @SpecialtyNum INT,
    @Locations VARCHAR(250),
    @AvailableSlots DATETIME,
    @Rating FLOAT
AS
BEGIN
INSERT INTO WORKER (WORKERID, NAME, SPECIALTYNUM, LOCATIONS, AVAILABLESLOTS, RATING)
VALUES (@WorkerID, @Name, @SpecialtyNum, @Locations, @AvailableSlots, @Rating)
END
GO

CREATE PROCEDURE GetAllWorkers
    AS
BEGIN
SELECT * FROM WORKER
END
GO

CREATE PROCEDURE DeleteRequest
    @RequestID INT
AS
BEGIN
DELETE FROM REQUEST WHERE REQUESTID = @RequestID
END
GO

CREATE PROCEDURE GetRequest
    @RequestID INT
AS
BEGIN
SELECT * FROM REQUEST WHERE REQUESTID = @RequestID
END
GO

CREATE PROCEDURE GetAllRequests
    AS
BEGIN
SELECT * FROM REQUEST
END
GO

-- Specialty Procedures
CREATE PROCEDURE CreateSpecialty
    @WorkerID INT,
    @TaskID INT
AS
BEGIN
INSERT INTO SPECIALTY (WORKERID, TASKID)
VALUES (@WorkerID, @TaskID)
END
GO

CREATE PROCEDURE DeleteSpecialty
    @WorkerID INT,
    @TaskID INT
AS
BEGIN
DELETE FROM SPECIALTY
WHERE WORKERID = @WorkerID AND TASKID = @TaskID
END
GO

CREATE PROCEDURE GetSpecialty
    @WorkerID INT,
    @TaskID INT
AS
BEGIN
SELECT * FROM SPECIALTY
WHERE WORKERID = @WorkerID AND TASKID = @TaskID
END
GO

CREATE PROCEDURE GetAllSpecialties
    AS
BEGIN
SELECT * FROM SPECIALTY
END
GO

-- Assignment Procedures
CREATE PROCEDURE CreateAssignment
    @WorkerID INT,
    @ClientID INT,
    @RequestID INT,
    @Status VARCHAR(100),
    @WorkerRating FLOAT,
    @ClientRating FLOAT,
    @Wage DECIMAL(10, 2) = NULL 
AS
BEGIN
    IF EXISTS (SELECT 1 FROM WORKER WHERE WORKERID = @WorkerID)
    AND EXISTS (SELECT 1 FROM CLIENT WHERE CLIENTID = @ClientID)
    AND EXISTS (SELECT 1 FROM REQUEST WHERE REQUESTID = @RequestID)
BEGIN
INSERT INTO ASSIGNMENT (WORKERID, CLIENTID, REQUESTID, STATUS, WORKERRATING, CLIENTRATING, WAGE)
VALUES (@WorkerID, @ClientID, @RequestID, @Status, @WorkerRating, @ClientRating, @Wage)
END
ELSE
        THROW 50002, 'Invalid WorkerID, ClientID, or RequestID', 1
END
GO

CREATE PROCEDURE UpdateAssignment
    @WorkerID INT,
    @ClientID INT,
    @RequestID INT,
    @Status VARCHAR(100),
    @WorkerRating FLOAT,
    @ClientRating FLOAT,
    @Wage DECIMAL(10, 2) = NULL
AS
BEGIN
UPDATE ASSIGNMENT
SET STATUS = @Status,
    WORKERRATING = @WorkerRating,
    CLIENTRATING = @ClientRating,
    WAGE = @Wage
WHERE WORKERID = @WorkerID
  AND CLIENTID = @ClientID
  AND REQUESTID = @REQUESTID
END
GO

CREATE PROCEDURE DeleteAssignment
    @WorkerID INT,
    @ClientID INT,
    @RequestID INT
AS
BEGIN
DELETE FROM ASSIGNMENT
WHERE WORKERID = @WorkerID
  AND CLIENTID = @ClientID
  AND REQUESTID = @REQUESTID
END
GO

CREATE PROCEDURE GetAssignment
    @WorkerID INT,
    @ClientID INT,
    @RequestID INT
AS
BEGIN
SELECT * FROM ASSIGNMENT
WHERE WORKERID = @WorkerID
  AND CLIENTID = @ClientID
  AND REQUESTID = @RequestID
END
GO

CREATE PROCEDURE GetAllAssignments
    AS
BEGIN
SELECT * FROM ASSIGNMENT
END
GO

-- For Specialties
CREATE PROCEDURE GetMostLeastPopularSpecialties
    AS
BEGIN
SELECT
    t.TaskID AS SpecialtyID,
    t.TaskName AS SpecialtyName,
    COUNT(r.RequestID) AS RequestCount,
    RANK() OVER (ORDER BY COUNT(r.RequestID) DESC) AS PopularityRank
FROM Task t
         LEFT JOIN Request r ON t.TaskID = r.TaskID
GROUP BY t.TaskID, t.TaskName;
END
GO

CREATE PROCEDURE GetBestRatedWorkerPerSpecialty
    @StartDate DATETIME,
    @EndDate DATETIME
AS
BEGIN
WITH RatingCTE AS (
    SELECT
        s.TaskID,
        a.WorkerID,
        w.Name,
        AVG(a.WorkerRating) AS AvgRating,
        ROW_NUMBER() OVER (PARTITION BY s.TaskID ORDER BY AVG(a.WorkerRating) DESC) AS Rank
    FROM Assignment a
             JOIN Request r ON a.RequestID = r.RequestID
             JOIN Specialty s ON a.WorkerID = s.WorkerID AND r.TaskID = s.TaskID
             JOIN Worker w ON a.WorkerID = w.WorkerID
    WHERE
        r.RequestTime BETWEEN @StartDate AND @EndDate
    GROUP BY s.TaskID, a.WorkerID, w.Name
)
SELECT
    TaskID,
    WorkerID,
    Name,
    AvgRating
FROM RatingCTE
WHERE Rank = 1;
END
GO

-- CLIENTS
INSERT INTO CLIENT (CLIENTID, NAME, PHONE, ADDRESS, EMAIL, OVERALLFEEDBACK, PASSWORD)
VALUES
(1, 'John Doe', '555-123-4567', '123 Main St, City', 'john.doe@email.com', 'Great client!', 'password123'),
(2, 'Jane Smith', '555-987-6543', '456 Oak Ave, Town', 'jane.smith@email.com', 'Reliable and responsive', 'securepass456');

-- TASKS
INSERT INTO TASK (TASKID, TASKNAME, AVGTIMETOFINISH, AVGFEE)
VALUES
(1, 'Plumbing Repair', 120, 150.00),
(2, 'Electrical Wiring', 180, 200.00),
(3, 'HVAC Maintenance', 150, 180.00),
(4, 'Roof Inspection', 90, 120.00),
(5, 'Landscaping', 240, 220.00),
(6, 'Painting', 200, 170.00);

-- WORKERS
INSERT INTO WORKER (WORKERID, REQUESTID, NAME, SPECIALTYNUM, LOCATIONS, AVAILABLESLOTS, RATING)
VALUES
(1, NULL, 'Mike Johnson', 2, 'City, Town', DATEADD(day, 1, GETDATE()), 4.5),
(2, NULL, 'Sarah Brown', 3, 'Town, Village', DATEADD(day, 2, GETDATE()), 4.2),
(3, NULL, 'David Lee', 2, 'City, Hamlet', DATEADD(day, 3, GETDATE()), 4.8),
(4, NULL, 'Emma Davis', 1, 'City', DATEADD(day, 4, GETDATE()), 4.1),
(5, NULL, 'Chris Wilson', 2, 'Village, Town', DATEADD(day, 5, GETDATE()), 3.9),
(6, NULL, 'Olivia Garcia', 2, 'Hamlet, City', DATEADD(day, 6, GETDATE()), 4.7);

-- REQUESTS (1-12, so we can assign each to a unique combination)
INSERT INTO REQUEST (REQUESTID, CLIENTID, TASKID, ADDRESS, REQUESTTIME, PREFERREDTIMESLOT, REQUESTDESCRIPTION)
VALUES
(1, 1, 1, '123 Main St, City', GETDATE(), DATEADD(day, 1, GETDATE()), 'Fix faucet'),
(2, 2, 2, '456 Oak Ave, Town', GETDATE(), DATEADD(day, 1, GETDATE()), 'Install ceiling fan'),
(3, 1, 3, '789 Pine Rd, Village', GETDATE(), DATEADD(day, 2, GETDATE()), 'HVAC annual check'),
(4, 2, 4, '321 Elm St, Hamlet', GETDATE(), DATEADD(day, 2, GETDATE()), 'Inspect roof leaks'),
(5, 1, 5, '654 Maple Ave, City', GETDATE(), DATEADD(day, 3, GETDATE()), 'Backyard redesign'),
(6, 2, 6, '987 Cedar Ln, Town', GETDATE(), DATEADD(day, 3, GETDATE()), 'Paint garage door'),
(7, 1, 1, '741 Spruce Ave, Town', GETDATE(), DATEADD(day, 4, GETDATE()), 'Unclog kitchen drain'),
(8, 2, 2, '369 Willow Rd, City', GETDATE(), DATEADD(day, 4, GETDATE()), 'Fix power outage'),
(9, 1, 3, '147 Birch St, Village', GETDATE(), DATEADD(day, 5, GETDATE()), 'AC not cooling'),
(10, 2, 4, '258 Oakwood Dr, Hamlet', GETDATE(), DATEADD(day, 5, GETDATE()), 'Loose roof tiles'),
(11, 1, 5, '369 Cedar Rd, Town', GETDATE(), DATEADD(day, 6, GETDATE()), 'Tree planting'),
(12, 2, 6, '159 Poplar Blvd, City', GETDATE(), DATEADD(day, 6, GETDATE()), 'Bedroom repainting');

-- ASSIGNMENTS (all completed)
INSERT INTO ASSIGNMENT (WORKERID, CLIENTID, REQUESTID, ACTUALTIMETAKEN, WORKERRATING, CLIENTRATING, STATUS, FEEDBACKBYCLIENT, FEEDBACKBYWORKER, WAGE)
VALUES
(1, 1, 1, 125, 4.7, 4.6, 'Completed', 'Great service!', 'Smooth experience.', 160.00),
(2, 2, 2, 170, 4.2, 4.0, 'Completed', 'On time and quick.', 'Nice client.', 195.00),
(3, 1, 3, 150, 4.9, 4.8, 'Completed', 'Efficient technician.', 'Client was helpful.', 185.00),
(4, 2, 4, 95, 4.0, 4.2, 'Completed', 'Clear explanation.', 'Responsive client.', 130.00),
(5, 1, 5, 230, 3.8, 3.9, 'Completed', 'Took longer than expected.', 'Cooperative client.', 210.00),
(6, 2, 6, 190, 4.6, 4.7, 'Completed', 'Clean and detailed.', 'Nice project.', 175.00),
(1, 1, 7, 110, 4.5, 4.4, 'Completed', 'Solved quickly.', 'Polite and clear.', 150.00),
(2, 2, 8, 160, 4.1, 4.0, 'Completed', 'Fixed in no time.', 'Thankful user.', 180.00),
(3, 1, 9, 145, 5.0, 4.9, 'Completed', 'Excellent diagnosis.', 'Great client.', 190.00),
(4, 2, 10, 100, 4.3, 4.2, 'Completed', 'Identified problem fast.', 'Communicative client.', 140.00),
(5, 1, 11, 250, 4.0, 4.1, 'Completed', 'Good landscaping.', 'Patient and friendly.', 220.00),
(6, 2, 12, 190, 4.8, 4.9, 'Completed', 'Clean paint job.', 'Detailed instructions.', 180.00);

-- SPECIALTIES (each worker has at least 2 tasks)
INSERT INTO SPECIALTY (WORKERID, TASKID)
VALUES
(1, 1), (1, 5),
(2, 2), (2, 6),
(3, 3), (3, 1),
(4, 4), (4, 6),
(5, 5), (5, 2),
(6, 6), (6, 3);

SELECT * FROM CLIENT;
SELECT * FROM TASK;
SELECT * FROM WORKER;
SELECT * FROM REQUEST;
SELECT * FROM SPECIALTY;
SELECT * FROM ASSIGNMENT;