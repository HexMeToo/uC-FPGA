library ieee;
use ieee.std_logic_1164.all;

entity Tablica_Znakow is
  port ( CLK: in std_logic;
			OE : in std_logic_vector(3 downto 0);
			A : in std_logic_vector(2 downto 0);
         Q : out std_logic_vector(7 downto 0));
end entity Tablica_Znakow;

architecture Behavioral of Tablica_Znakow is
signal Q0: std_logic_vector(7 downto 0);
signal Q1: std_logic_vector(7 downto 0);
signal Q2: std_logic_vector(7 downto 0);
signal Q3: std_logic_vector(7 downto 0);
signal Q4: std_logic_vector(7 downto 0);
signal Q5: std_logic_vector(7 downto 0);
signal Q6: std_logic_vector(7 downto 0);
signal Q7: std_logic_vector(7 downto 0);
signal Q8: std_logic_vector(7 downto 0);
signal Q9: std_logic_vector(7 downto 0);
signal Q10: std_logic_vector(7 downto 0);
signal Q11: std_logic_vector(7 downto 0);
signal Q12: std_logic_vector(7 downto 0);
signal Q13: std_logic_vector(7 downto 0);
signal Q14: std_logic_vector(7 downto 0);
signal Q15: std_logic_vector(7 downto 0);
begin

	IC0: entity work.ROM_0 port map
	(
		A => A,
		Q => Q0
	);

	IC1: entity work.ROM_1 port map
	(
		A => A,
		Q => Q1
	);
	
	IC2: entity work.ROM_2 port map
	(
		A => A,
		Q => Q2
	);
	
	IC3: entity work.ROM_3 port map
	(
		A => A,
		Q => Q3
	);
	
	IC4: entity work.ROM_4 port map
	(
		A => A,
		Q => Q4
	);
	
	IC5: entity work.ROM_5 port map
	(
		A => A,
		Q => Q5
	);
	
	IC6: entity work.ROM_6 port map
	(
		A => A,
		Q => Q6
	);
	
	IC7: entity work.ROM_7 port map
	(
		A => A,
		Q => Q7
	);
	
	IC8: entity work.ROM_8 port map
	(
		A => A,
		Q => Q8
	);
	
	IC9: entity work.ROM_9 port map
	(
		A => A,
		Q => Q9
	);
	
	IC10: entity work.ROM_G port map
	(
		A => A,
		Q => Q10
	);
	
	IC11: entity work.ROM_J port map
	(
		A => A,
		Q => Q11
	);
	
	IC12: entity work.ROM_Q port map
	(
		A => A,
		Q => Q12
	);
	
	IC13: entity work.ROM_K port map
	(
		A => A,
		Q => Q13
	);
	
	IC14: entity work.ROM_A port map
	(
		A => A,
		Q => Q14
	);
	
	IC15: entity work.ROM_W port map
	(
		A => A,
		Q => Q15
	);

	process(CLK, OE)
	begin
		if clk'event and clk = '1' then
			case OE is
				when "0000" =>		Q <= Q0;
				when "0001" =>		Q <= Q1;
				when "0010" =>		Q <= Q2;
				when "0011" =>		Q <= Q3;
				when "0100" =>		Q <= Q4;
				when "0101" =>		Q <= Q5;
				when "0110" =>		Q <= Q6;
				when "0111" =>		Q <= Q7;
				when "1000" =>		Q <= Q8;
				when "1001" =>		Q <= Q9;
				when "1010" =>		Q <= Q10;
				when "1011" =>		Q <= Q11;
				when "1100" =>		Q <= Q12;
				when "1101" =>		Q <= Q13;
				when "1110" =>		Q <= Q14;
				when "1111" =>		Q <= Q15;
			end case;
		end if;
	end process;

end architecture Behavioral;