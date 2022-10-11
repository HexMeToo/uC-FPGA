library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.std_logic_unsigned.all;

entity RAM is
	port(
			D: in std_logic_vector(7 downto 0);
			A: in std_logic_vector(2 downto 0);
			CE: in std_logic;
			Q: out std_logic_vector(7 downto 0));
end RAM;

architecture Behavioral of RAM is
signal R0: std_logic_vector(7 downto 0) := "00000000";
signal R1: std_logic_vector(7 downto 0) := "00000000";
signal R2: std_logic_vector(7 downto 0) := "00000000";
signal R3: std_logic_vector(7 downto 0) := "00000000";
signal R4: std_logic_vector(7 downto 0) := "00000000";
signal R5: std_logic_vector(7 downto 0) := "00000000";
signal R6: std_logic_vector(7 downto 0) := "00000000";
signal R7: std_logic_vector(7 downto 0) := "00000000";

begin
	process(A, CE, D)
	begin
		if CE = '1' then
			case A is
				when "000" =>   Q <= R0;
				when "001" =>   Q <= R1;
				when "010" =>   Q <= R2;
				when "011" =>   Q <= R3;
				when "100" =>   Q <= R4;
				when "101" =>   Q <= R5;
				when "110" =>   Q <= R6;
				when "111" =>   Q <= R7;
			end case;
		else
			case A is
				when "000" =>   R0 <= D;
				when "001" =>   R1 <= D;
				when "010" =>   R2 <= D;
				when "011" =>   R3 <= D;
				when "100" =>   R4 <= D;
				when "101" =>   R5 <= D;
				when "110" =>   R6 <= D;
				when "111" =>   R7 <= D;
			end case;
		end if;
	end process;
	
end Behavioral;
