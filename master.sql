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
                            FEEDBACKBYWORKER     text                 null
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

/* views */

/*==============================================================*/
/* View: MatchingWorkersView                                    */
/*==============================================================*/
CREATE VIEW MatchingWorkersView AS
SELECT r.RequestID, w.WorkerID, w.Name, w.Locations, w.AvailableSlots
FROM Request r
         JOIN Task t ON r.TaskID = t.TaskID
         JOIN Specialty s ON t.TaskID = s.TaskID
         JOIN Worker w ON s.WorkerID = w.WorkerID
WHERE w.Locations LIKE '%' + r.Address + '%'
  AND w.AvailableSlots >= r.PreferredTimeSlot;
GO

/*==============================================================*/
/* View: WorkerTotalWageView                                    */
/*==============================================================*/
CREATE VIEW WorkerTotalWageView AS
SELECT a.WorkerID, w.Name, SUM(t.AvgFee * a.WorkerRating / 5) AS TotalDueWage
FROM Assignment a
         JOIN Request r ON a.RequestID = r.RequestID
         JOIN Task t ON r.TaskID = t.TaskID
         JOIN Worker w ON a.WorkerID = w.WorkerID
WHERE a.Status = 'Completed'
GROUP BY a.WorkerID, w.Name;
GO

/*==============================================================*/
/* View: TaskRequestCountView                                   */
/*==============================================================*/
CREATE VIEW TaskRequestCountView AS
SELECT t.TaskID, t.TaskName, COUNT(r.RequestID) AS RequestCount
FROM Task t
         LEFT JOIN Request r ON t.TaskID = r.TaskID
GROUP BY t.TaskID, t.TaskName;
GO

/*==============================================================*/
/* View: WorkerAssignmentCountView                              */
/*==============================================================*/
CREATE VIEW WorkerAssignmentCountView AS
SELECT w.WorkerID, w.Name, COUNT(a.RequestID) AS AssignmentCount
FROM Worker w
         LEFT JOIN Assignment a ON w.WorkerID = a.WorkerID
GROUP BY w.WorkerID, w.Name;
GO

/*==============================================================*/
/* View: SpecialtyBestWorkerView                                */
/*==============================================================*/
CREATE VIEW SpecialtyBestWorkerView AS
SELECT s.TaskID, a.WorkerID, AVG(a.WorkerRating) AS AvgRating
FROM Assignment a
         JOIN Request r ON a.RequestID = r.RequestID
         JOIN Specialty s ON a.WorkerID = s.WorkerID AND r.TaskID = s.TaskID
GROUP BY s.TaskID, a.WorkerID;
GO

/*==============================================================*/
/* View: FeaturedTasksView                                      */
/*==============================================================*/
CREATE VIEW FeaturedTasksView AS
SELECT TOP 5 t.TaskID, t.TaskName, t.AvgFee, COUNT(r.RequestID) AS RequestCount
FROM Task t
         LEFT JOIN Request r ON t.TaskID = r.TaskID
GROUP BY t.TaskID, t.TaskName, t.AvgFee
ORDER BY RequestCount DESC;
GO

/*==============================================================*/
/* View: AvailableWorkersView                                   */
/*==============================================================*/
CREATE VIEW AvailableWorkersView AS
SELECT WorkerID, Name, Rating, Locations
FROM Worker
WHERE AvailableSlots >= GETDATE();
GO

/*==============================================================*/
/* View: OfferedTasksView                                       */
/*==============================================================*/
CREATE VIEW OfferedTasksView AS
SELECT TaskID, TaskName, AvgTimeToFinish, AvgFee
FROM Task;
GO

/*==============================================================*/
/* View: FeedbackView                                           */
/*==============================================================*/
CREATE VIEW FeedbackView AS
SELECT a.RequestID, c.Name AS ClientName, w.Name AS WorkerName, a.FeedbackByClient, a.WorkerRating
FROM Assignment a
         JOIN Client c ON a.ClientID = c.ClientID
         JOIN Worker w ON a.WorkerID = w.WorkerID;
GO

/*==============================================================*/
/* View: WorkerOverallRatingView                                */
/*==============================================================*/
CREATE VIEW WorkerOverallRatingView AS
SELECT w.WorkerID, w.Name, AVG(a.WorkerRating) AS OverallRating
FROM Worker w
         LEFT JOIN Assignment a ON w.WorkerID = a.WorkerID
GROUP BY w.WorkerID, w.Name;
GO

/*==============================================================*/
/* View: TopRatedWorkersView                                    */
/*==============================================================*/
CREATE VIEW TopRatedWorkersView AS
SELECT w.WorkerID, w.Name
FROM Worker w
WHERE NOT EXISTS (
    SELECT 1
    FROM Assignment a
    WHERE a.WorkerID = w.WorkerID
      AND (a.WorkerRating < 4.5 OR a.WorkerRating IS NULL)
);
GO

/*==============================================================*/
/* View: ClientRequestHistoryView                               */
/*==============================================================*/
CREATE VIEW ClientRequestHistoryView AS
SELECT r.ClientID, r.RequestID, t.TaskName, r.RequestTime, a.Status
FROM Request r
         JOIN Assignment a ON r.RequestID = a.RequestID
         JOIN Task t ON r.TaskID = t.TaskID;
GO

/*==============================================================*/
/* View: UnrequestedSpecialtiesView                             */
/*==============================================================*/
CREATE VIEW UnrequestedSpecialtiesView AS
SELECT t.TaskID, t.TaskName
FROM Task t
WHERE NOT EXISTS (
    SELECT 1
    FROM Request r
    WHERE r.TaskID = t.TaskID
              AND YEAR(r.RequestTime) = YEAR(GETDATE())
    AND MONTH(r.RequestTime) = MONTH(GETDATE())
);
GO



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

CREATE PROCEDURE UpdateWorker
    @WorkerID INT,
    @Name VARCHAR(100),
    @SpecialtyNum INT,
    @Locations VARCHAR(250),
    @AvailableSlots DATETIME,
    @Rating FLOAT
AS
BEGIN
UPDATE WORKER
SET NAME = @Name,
    SPECIALTYNUM = @SpecialtyNum,
    LOCATIONS = @Locations,
    AVAILABLESLOTS = @AvailableSlots,
    RATING = @Rating
WHERE WORKERID = @WorkerID
END
GO

CREATE PROCEDURE DeleteWorker
    @WorkerID INT
AS
BEGIN
DELETE FROM WORKER WHERE WORKERID = @WorkerID
END
GO

CREATE PROCEDURE GetWorker
    @WorkerID INT
AS
BEGIN
SELECT * FROM WORKER WHERE WORKERID = @WorkerID
END
GO

CREATE PROCEDURE GetAllWorkers
    AS
BEGIN
SELECT * FROM WORKER
END
GO

-- Task Procedures
CREATE PROCEDURE CreateTask
    @TaskID INT,
    @TaskName VARCHAR(100),
    @AvgTimeToFinish INT,
    @AvgFee FLOAT
AS
BEGIN
INSERT INTO TASK (TASKID, TASKNAME, AVGTIMETOFINISH, AVGFEE)
VALUES (@TaskID, @TaskName, @AvgTimeToFinish, @AvgFee)
END
GO

CREATE PROCEDURE UpdateTask
    @TaskID INT,
    @TaskName VARCHAR(100),
    @AvgTimeToFinish INT,
    @AvgFee FLOAT
AS
BEGIN
UPDATE TASK
SET TASKNAME = @TaskName,
    AVGTIMETOFINISH = @AvgTimeToFinish,
    AVGFEE = @AvgFee
WHERE TASKID = @TaskID
END
GO

CREATE PROCEDURE DeleteTask
    @TaskID INT
AS
BEGIN
DELETE FROM TASK WHERE TASKID = @TaskID
END
GO

CREATE PROCEDURE GetTask
    @TaskID INT
AS
BEGIN
SELECT * FROM TASK WHERE TASKID = @TaskID
END
GO

CREATE PROCEDURE GetAllTasks
    AS
BEGIN
SELECT * FROM TASK
END
GO

-- Request Procedures
CREATE PROCEDURE CreateRequest
    @RequestID INT,
    @ClientID INT,
    @TaskID INT,
    @Address VARCHAR(250),
    @PreferredTimeSlot DATETIME
AS
BEGIN
    IF EXISTS (SELECT 1 FROM CLIENT WHERE CLIENTID = @ClientID)
    AND EXISTS (SELECT 1 FROM TASK WHERE TASKID = @TaskID)
BEGIN
INSERT INTO REQUEST (REQUESTID, CLIENTID, TASKID, ADDRESS, PREFERREDTIMESLOT)
VALUES (@RequestID, @ClientID, @TaskID, @Address, @PreferredTimeSlot)
END
ELSE
        THROW 50001, 'Invalid ClientID or TaskID', 1
END
GO

CREATE PROCEDURE UpdateRequest
    @RequestID INT,
    @ClientID INT,
    @TaskID INT,
    @Address VARCHAR(250),
    @PreferredTimeSlot DATETIME,
    @RequestTime DATETIME,
    @RequestDescription VARCHAR(250)
AS
BEGIN
UPDATE REQUEST
SET CLIENTID = @ClientID,
    TASKID = @TaskID,
    ADDRESS = @Address,
    PREFERREDTIMESLOT = @PreferredTimeSlot,
    REQUESTTIME         = @RequestTime,
    REQUESTDESCRIPTION  = @RequestDescription
WHERE REQUESTID = @RequestID
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
    @ClientRating FLOAT
AS
BEGIN
    IF EXISTS (SELECT 1 FROM WORKER WHERE WORKERID = @WorkerID)
    AND EXISTS (SELECT 1 FROM CLIENT WHERE CLIENTID = @ClientID)
    AND EXISTS (SELECT 1 FROM REQUEST WHERE REQUESTID = @RequestID)
BEGIN
INSERT INTO ASSIGNMENT (WORKERID, CLIENTID, REQUESTID, STATUS, WORKERRATING, CLIENTRATING)
VALUES (@WorkerID, @ClientID, @RequestID, @Status, @WorkerRating, @ClientRating)
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
    @ClientRating FLOAT
AS
BEGIN
UPDATE ASSIGNMENT
SET STATUS = @Status,
    WORKERRATING = @WorkerRating,
    CLIENTRATING = @ClientRating
WHERE WORKERID = @WorkerID
  AND CLIENTID = @ClientID
  AND REQUESTID = @RequestID
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
  AND REQUESTID = @RequestID
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

-- Business Logic Procedures
CREATE PROCEDURE MatchWorkersToRequest
    @RequestID INT
AS
BEGIN
SELECT w.WorkerID, w.Name, w.Locations, w.AvailableSlots
FROM Worker w
         JOIN Specialty s ON w.WorkerID = s.WorkerID
         JOIN Request r ON s.TaskID = r.TaskID
WHERE r.RequestID = @RequestID
  AND w.Locations LIKE '%' + r.Address + '%'
  AND w.AvailableSlots >= r.PreferredTimeSlot
END
GO

CREATE PROCEDURE CalculateOverallRating
    @WorkerID INT
AS
BEGIN
SELECT AVG(WorkerRating) AS OverallRating
FROM Assignment
WHERE WorkerID = @WorkerID
END
GO

CREATE PROCEDURE GetMatchingWorkersForRequest
    @RequestID INT
AS
BEGIN
SELECT
    w.WorkerID,
    w.Name,
    w.Locations,
    w.AvailableSlots,
    t.TaskName AS RequiredSpecialty
FROM Worker w
         JOIN Specialty s ON w.WorkerID = s.WorkerID
         JOIN Request r ON s.TaskID = r.TaskID
         JOIN Task t ON r.TaskID = t.TaskID
WHERE
    r.RequestID = @RequestID
  AND w.Locations LIKE '%' + r.Address + '%'
  AND w.AvailableSlots >= r.PreferredTimeSlot
ORDER BY w.Rating DESC;
END
GO

CREATE PROCEDURE CalculateTotalDueWage
    @StartDate DATETIME,
    @EndDate DATETIME
AS
BEGIN
SELECT
    a.WorkerID,
    w.Name,
    SUM(t.AvgFee * (a.WorkerRating / 5.0)) AS TotalDueWage
FROM Assignment a
         JOIN Request r ON a.RequestID = r.RequestID
         JOIN Task t ON r.TaskID = t.TaskID
         JOIN Worker w ON a.WorkerID = w.WorkerID
WHERE
    a.Status = 'Completed'
  AND r.RequestTime BETWEEN @StartDate AND @EndDate
GROUP BY a.WorkerID, w.Name;
END
GO

-- For Tasks
CREATE PROCEDURE GetMostLeastRequestedTasks
    AS
BEGIN
SELECT
    t.TaskID,
    t.TaskName,
    COUNT(RequestID) AS RequestCount,
    RANK() OVER (ORDER BY COUNT(RequestID) DESC) AS PopularityRank
FROM Request r
         JOIN Task t ON r.TaskID = t.TaskID
GROUP BY t.TaskID, t.TaskName;
END
GO

-- For Workers
CREATE PROCEDURE GetMostLeastActiveWorkers
    AS
BEGIN
SELECT
    w.WorkerID,
    w.Name,
    COUNT(a.RequestID) AS AssignmentCount,
    RANK() OVER (ORDER BY COUNT(a.RequestID) DESC) AS ActivityRank
FROM Worker w
         LEFT JOIN Assignment a ON w.WorkerID = a.WorkerID
GROUP BY w.WorkerID, w.Name;
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

CREATE OR ALTER PROCEDURE CreateRequest
    @RequestID INT,
    @ClientID INT,
    @TaskID INT,
    @Address VARCHAR(250),
    @PreferredTimeSlot DATETIME
AS
BEGIN
    IF EXISTS (SELECT 1 FROM CLIENT WHERE CLIENTID = @ClientID)
    AND EXISTS (SELECT 1 FROM TASK WHERE TASKID = @TaskID)
    BEGIN
        INSERT INTO REQUEST (REQUESTID, CLIENTID, TASKID, ADDRESS, REQUESTTIME, PREFERREDTIMESLOT)
        VALUES (@RequestID, @ClientID, @TaskID, @Address, GETDATE(), @PreferredTimeSlot)
    END
    ELSE
        THROW 50001, 'Invalid ClientID or TaskID', 1
END
GO

INSERT INTO CLIENT (CLIENTID, NAME, PHONE, ADDRESS, EMAIL, OVERALLFEEDBACK, PASSWORD)
VALUES 
(1, 'John Doe', '555-123-4567', '123 Main St, City', 'john.doe@email.com', 'Great client!', 'password123'),
(2, 'Jane Smith', '555-987-6543', '456 Oak Ave, Town', 'jane.smith@email.com', NULL, 'securepass456');

INSERT INTO TASK (TASKID, TASKNAME, AVGTIMETOFINISH, AVGFEE)
VALUES 
(1, 'Plumbing Repair', 120, 150.00), -- 120 minutes, $150 avg fee
(2, 'Electrical Wiring', 180, 200.00); -- 180 minutes, $200 avg fee

INSERT INTO WORKER (WORKERID, REQUESTID, NAME, SPECIALTYNUM, LOCATIONS, AVAILABLESLOTS, RATING)
VALUES 
(1, NULL, 'Mike Johnson', 2, 'City, Town', DATEADD(day, 1, GETDATE()), 4.5), -- Available tomorrow
(2, NULL, 'Sarah Brown', 1, 'City', DATEADD(day, 2, GETDATE()), 4.0); -- Available in 2 days

INSERT INTO REQUEST (REQUESTID, CLIENTID, TASKID, ADDRESS, REQUESTTIME, PREFERREDTIMESLOT, REQUESTDESCRIPTION)
VALUES 
(1, 1, 1, '123 Main St, City', GETDATE(), DATEADD(day, 1, GETDATE()), 'Fix leaking pipe in kitchen'),
(2, 2, 2, '456 Oak Ave, Town', GETDATE(), DATEADD(day, 2, GETDATE()), 'Install new lighting in living room');

INSERT INTO SPECIALTY (WORKERID, TASKID)
VALUES 
(1, 1), -- Mike Johnson specializes in Plumbing Repair
(2, 2); -- Sarah Brown specializes in Electrical Wiring

INSERT INTO ASSIGNMENT (WORKERID, CLIENTID, REQUESTID, ACTUALTIMETAKEN, WORKERRATING, CLIENTRATING, STATUS, FEEDBACKBYCLIENT, FEEDBACKBYWORKER)
VALUES 
(1, 1, 1, 130, 4.8, 4.5, 'Completed', 'Great job fixing the pipe!', 'Client was friendly.'),
(2, 2, 2, NULL, NULL, NULL, 'In Progress', NULL, NULL);

SELECT * FROM CLIENT;
SELECT * FROM TASK;
SELECT * FROM WORKER;
SELECT * FROM REQUEST;
SELECT * FROM SPECIALTY;
SELECT * FROM ASSIGNMENT;
