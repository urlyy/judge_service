# 没用到，先留着
import sys

def check_syntax(file_path):
    try:
        with open(file_path, 'r') as file:
            script = file.read()
        # 尝试编译脚本
        compile(script, file_path, 'exec')
        print(f"The script '{file_path}' has no syntax errors.")
        return True
    except SyntaxError as e:
        # 捕获语法错误
        print(f"Syntax error in '{file_path}': {e}")
        return False

if __name__ == "__main__":
    file_path = sys.argv[1] if len(sys.argv) > 1 else "data/code.py"
    # 检查文件语法
    check_syntax(file_path)
