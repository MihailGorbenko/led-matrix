#ifndef MATRIX_TASK_HPP
#define MATRIX_TASK_HPP

class MatrixTask {
public:
    virtual ~MatrixTask() = default;

    // Метод для запуска задачи
    virtual void startTask() = 0;

    // Метод для остановки задачи
    virtual void stopTask() = 0;
};

#endif // MATRIX_TASK_HPP